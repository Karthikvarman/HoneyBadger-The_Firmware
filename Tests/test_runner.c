#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../Config/wf_config.h"
#include "../Config/sensor_config.h"
#include "../Config/pid_config.h"
#include "../Middleware/crc.h"
#include "../Middleware/bus_arbiter.h"
#include "../Middleware/data_store.h"
#include "../Middleware/pid_controller.h"
#include "../Middleware/packet_builder.h"
#include "../Middleware/packet_parser.h"
#include "../Middleware/scheduler.h"

static int g_tests_run = 0;
static int g_tests_passed = 0;

#define TEST_ASSERT(cond, msg) do { \
    g_tests_run++; \
    if (cond) { \
        g_tests_passed++; \
        printf("  [PASS] %s\n", msg); \
    } else { \
        printf("  [FAIL] %s (Line %d)\n", msg, __LINE__); \
    } \
} while(0)

static void Test_CRC(void) {
    printf("\n=== Running CRC Tests ===\n");
    const uint8_t test_data[] = "123456789";
    uint16_t crc16 = CRC16_CCITT(test_data, 9);
    TEST_ASSERT(crc16 != 0, "CRC16-CCITT computed non-zero hash");

    uint8_t cmd_payload[5] = { WF_CMD_ID_SET_POS, 0x10, 0x27, 0x00, 0x00 };
    uint8_t crc8 = CRC8_Calculate(cmd_payload, 5);
    TEST_ASSERT(crc8 != 0, "CRC8 computed non-zero hash for command payload");
}

static void Test_BusArbiter(void) {
    printf("\n=== Running Bus Arbiter Tests ===\n");
    BusArbiter_Init();

    TEST_ASSERT(!BusArbiter_IsLocked(WF_BUS_SPI1), "SPI1 initially unlocked");
    TEST_ASSERT(BusArbiter_Lock(WF_BUS_SPI1, (uint8_t)WF_SENSOR_S3) == HAL_OK, "S3 locks SPI1");
    TEST_ASSERT(BusArbiter_IsLocked(WF_BUS_SPI1), "SPI1 is locked");
    TEST_ASSERT(BusArbiter_Lock(WF_BUS_SPI1, (uint8_t)WF_SENSOR_S4) == HAL_BUSY, "S4 rejected when S3 holds SPI1 lock");
    TEST_ASSERT(BusArbiter_Unlock(WF_BUS_SPI1, (uint8_t)WF_SENSOR_S4) == HAL_ERROR, "S4 cannot unlock S3's lock");
    TEST_ASSERT(BusArbiter_Unlock(WF_BUS_SPI1, (uint8_t)WF_SENSOR_S3) == HAL_OK, "S3 releases SPI1 lock");
    TEST_ASSERT(!BusArbiter_IsLocked(WF_BUS_SPI1), "SPI1 unlocked after S3 release");
}

static void Test_DataStore(void) {
    printf("\n=== Running Data Store Double-Buffer Tests ===\n");
    DataStore_Init();

    uint8_t s1_dummy[2] = { 0xAA, 0x55 };
    DataStore_WriteSensor(WF_SENSOR_S1, WF_SENSOR_OK, s1_dummy, 2, 100);

    DataStore_SwapBuffers();

    const WF_SensorData_t *snap_s1 = DataStore_GetSnapshotSensor(WF_SENSOR_S1);
    TEST_ASSERT(snap_s1 != NULL, "Snapshot S1 entry retrieved");
    TEST_ASSERT(snap_s1->id == WF_SENSOR_S1, "S1 ID matches");
    TEST_ASSERT(snap_s1->status == WF_SENSOR_OK, "S1 status is OK");
    TEST_ASSERT(snap_s1->data[0] == 0xAA && snap_s1->data[1] == 0x55, "S1 data preserved through swap");
}

static void Test_PID(void) {
    printf("\n=== Running PID Controller Tests ===\n");
    WF_PIDState_t pid;
    PID_Init(&pid, WF_PID_KP, WF_PID_KI, WF_PID_KD);
    PID_SetSetpoint(&pid, 1000);

    int32_t out1 = PID_Update(&pid, 0);
    TEST_ASSERT(out1 > 0, "PID output is positive when position < setpoint");
    TEST_ASSERT(out1 <= WF_PID_OUTPUT_MAX, "PID output clamped within maximum PWM limit");

    int32_t out2 = PID_Update(&pid, 1000);
    TEST_ASSERT(out2 < out1, "PID output decreases when position reaches setpoint");

    HAL_Status_t clamp_res = PID_SetSetpoint(&pid, WF_MOTOR_POS_MAX + 5000);
    TEST_ASSERT(clamp_res == HAL_ERROR, "Out-of-bounds setpoint rejected and clamped");
    TEST_ASSERT(pid.setpoint == WF_MOTOR_POS_MAX, "Setpoint clamped to WF_MOTOR_POS_MAX");
}

static void Test_PacketBuilder(void) {
    printf("\n=== Running Packet Builder Tests ===\n");
    PacketBuilder_Init();
    DataStore_Init();

    uint8_t sample[2] = { 0x12, 0x34 };
    DataStore_WriteSensor(WF_SENSOR_S1, WF_SENSOR_OK, sample, 2, 500);
    DataStore_SwapBuffers();

    uint8_t pkt[WF_TELEMETRY_PKT_MAX];
    uint16_t pkt_len = 0;
    HAL_Status_t res = PacketBuilder_BuildTelemetry(pkt, sizeof(pkt), &pkt_len);

    TEST_ASSERT(res == HAL_OK, "PacketBuilder successfully assembled frame");
    TEST_ASSERT(pkt_len > 0, "Packet length is greater than 0");
    TEST_ASSERT(pkt[0] == WF_PKT_SOF, "SOF byte is 0xAA");
    TEST_ASSERT(pkt[1] == WF_PKT_ID_TELEMETRY, "Packet ID is 0x01");
    TEST_ASSERT(pkt[pkt_len - 1] == WF_PKT_EOF, "EOF byte is 0x55");
}

static void Test_PacketParser(void) {
    printf("\n=== Running Packet Parser Tests ===\n");
    PacketParser_Init();

    uint8_t cmd_payload[5] = { 0x01, 0x10, 0x27, 0x00, 0x00 };
    uint8_t crc = CRC8_Calculate(cmd_payload, 5);

    uint8_t valid_packet[8] = {
        WF_CMD_SOF,
        0x01,
        0x10, 0x27, 0x00, 0x00,
        crc,
        WF_CMD_EOF
    };

    int32_t target_pos = 0;
    int8_t feed_res = 0;
    for (int i = 0; i < 8; i++) {
        feed_res = PacketParser_FeedByte(valid_packet[i], &target_pos);
    }
    TEST_ASSERT(feed_res == 1, "Packet parser accepted valid motor command");
    TEST_ASSERT(target_pos == 10000, "Decoded target position equals 10000 counts");

    uint8_t bad_packet[8];
    memcpy(bad_packet, valid_packet, 8);
    bad_packet[6] ^= 0xFF;

    PacketParser_Init();
    for (int i = 0; i < 8; i++) {
        feed_res = PacketParser_FeedByte(bad_packet[i], &target_pos);
    }
    TEST_ASSERT(feed_res == -1, "Packet parser rejected packet with corrupted CRC");
}

static int s1_run_count = 0;
static int tx_run_count = 0;
static void Dummy_S1(void) { s1_run_count++; }
static void Dummy_TX(void) { tx_run_count++; }

static void Test_Scheduler(void) {
    printf("\n=== Running Scheduler Tests ===\n");
    Scheduler_Init();
    s1_run_count = 0;
    tx_run_count = 0;

    Scheduler_AddTask("S1", 10, 1, Dummy_S1);
    Scheduler_AddTask("TX", 10, 0, Dummy_TX);

    TEST_ASSERT(Scheduler_GetTaskCount() == 2, "2 tasks registered in scheduler");

    const WF_Task_t *table = Scheduler_GetTaskTable();
    TEST_ASSERT(table[0].priority == 0, "Priority 0 task sorted first in dispatch table");
    TEST_ASSERT(table[1].priority == 1, "Priority 1 task sorted second");

    for (int t = 0; t < 10; t++) {
        Scheduler_TickHook();
    }

    Scheduler_Dispatch();
    TEST_ASSERT(s1_run_count == 1, "S1 executed after 10ms tick");
    TEST_ASSERT(tx_run_count == 1, "TX executed after 10ms tick");
}

int main(void) {
    printf("==================================================\n");
    printf("      HoneyBadger Firmware Automated Test Suite     \n");
    printf("==================================================\n");

    Test_CRC();
    Test_BusArbiter();
    Test_DataStore();
    Test_PID();
    Test_PacketBuilder();
    Test_PacketParser();
    Test_Scheduler();

    printf("\n--------------------------------------------------\n");
    printf("Results: %d / %d tests PASSED (%.1f%%)\n",
           g_tests_passed, g_tests_run, (100.0f * g_tests_passed) / g_tests_run);
    printf("==================================================\n");

    return (g_tests_passed == g_tests_run) ? 0 : 1;
}
