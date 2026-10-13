#include "packet_builder.h"
#include "crc.h"
#include "../HAL/hal_timer.h"
#include <string.h>

static uint8_t g_seq_num = 0;

void PacketBuilder_Init(void) {
    g_seq_num = 0;
}

uint8_t PacketBuilder_GetSeqNum(void) {
    return g_seq_num;
}

HAL_Status_t PacketBuilder_BuildTelemetry(uint8_t *out_buf,
                                          uint16_t max_buf_len,
                                          uint16_t *out_pkt_len) {
    if (out_buf == (uint8_t*)0 || out_pkt_len == (uint16_t*)0 || max_buf_len < 32) {
        return HAL_ERROR;
    }

    uint16_t idx = 0;

    out_buf[idx++] = WF_PKT_SOF;

    uint16_t crc_start_idx = idx;
    out_buf[idx++] = WF_PKT_ID_TELEMETRY;

    out_buf[idx++] = g_seq_num++;

    uint16_t length_offset = idx;
    idx += 2;

    uint32_t timestamp = HAL_GetTick();
    out_buf[idx++] = (uint8_t)(timestamp & 0xFF);
    out_buf[idx++] = (uint8_t)((timestamp >> 8) & 0xFF);
    out_buf[idx++] = (uint8_t)((timestamp >> 16) & 0xFF);
    out_buf[idx++] = (uint8_t)((timestamp >> 24) & 0xFF);

    uint16_t payload_start = idx;

    for (uint8_t s = (uint8_t)WF_SENSOR_S1; s <= (uint8_t)WF_SENSOR_S10; s++) {
        const WF_SensorData_t *sens = DataStore_GetSnapshotSensor((WF_SensorID_t)s);
        if (sens == (const WF_SensorData_t*)0) {
            continue;
        }

        if (idx + 3 + sens->data_len + 3 > max_buf_len) {
            return HAL_ERROR;
        }

        out_buf[idx++] = (uint8_t)sens->id;
        out_buf[idx++] = (uint8_t)sens->status;
        out_buf[idx++] = sens->data_len;

        if (sens->data_len > 0) {
            memcpy(&out_buf[idx], sens->data, sens->data_len);
            idx += sens->data_len;
        }
    }

    uint16_t payload_len = idx - payload_start;
    out_buf[length_offset]     = (uint8_t)(payload_len & 0xFF);
    out_buf[length_offset + 1] = (uint8_t)((payload_len >> 8) & 0xFF);

    uint16_t crc_len = idx - crc_start_idx;
    uint16_t crc = CRC16_CCITT(&out_buf[crc_start_idx], crc_len);

    out_buf[idx++] = (uint8_t)(crc & 0xFF);
    out_buf[idx++] = (uint8_t)((crc >> 8) & 0xFF);

    out_buf[idx++] = WF_PKT_EOF;

    *out_pkt_len = idx;
    return HAL_OK;
}
