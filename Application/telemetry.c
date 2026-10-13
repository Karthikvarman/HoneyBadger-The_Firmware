#include "telemetry.h"
#include "motor_control.h"
#include "../Config/wf_config.h"
#include "../Config/pid_config.h"
#include "../HAL/hal_uart.h"
#include "../HAL/hal_timer.h"
#include "../Middleware/data_store.h"
#include "../Middleware/packet_builder.h"
#include "../Middleware/packet_parser.h"

static uint8_t  tx_packet_buffer[WF_TELEMETRY_PKT_MAX];
static uint32_t last_cmd_timestamp_ms = 0;

void Telemetry_Init(void) {
    PacketBuilder_Init();
    PacketParser_Init();
    last_cmd_timestamp_ms = HAL_GetTick();
}

void Task_Telemetry_TX(void) {

    DataStore_SwapBuffers();

    uint16_t pkt_len = 0;
    HAL_Status_t status = PacketBuilder_BuildTelemetry(tx_packet_buffer,
                                                       sizeof(tx_packet_buffer),
                                                       &pkt_len);

    if (status == HAL_OK && pkt_len > 0) {

        if (HAL_UART_TxComplete(HAL_UART_3)) {
            HAL_UART_Transmit_DMA(HAL_UART_3, tx_packet_buffer, pkt_len);
        } else {
            HAL_UART_Transmit(HAL_UART_3, tx_packet_buffer, pkt_len, 50);
        }
    }
}

void Telemetry_ProcessRX(void) {

    uint8_t rx_byte;
    while (HAL_UART_Receive(HAL_UART_3, &rx_byte, 1) > 0) {
        int32_t new_target_pos = 0;
        int8_t res = PacketParser_FeedByte(rx_byte, &new_target_pos);

        if (res == 1) {

            MotorControl_SetTargetPosition(new_target_pos);
            last_cmd_timestamp_ms = HAL_GetTick();
        }
    }
}

uint32_t Telemetry_GetLastCmdTimestamp(void) {
    return last_cmd_timestamp_ms;
}
