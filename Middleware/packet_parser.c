#include "packet_parser.h"
#include "crc.h"

typedef enum {
    STATE_WAIT_SOF,
    STATE_WAIT_CMD,
    STATE_WAIT_POS_0,
    STATE_WAIT_POS_1,
    STATE_WAIT_POS_2,
    STATE_WAIT_POS_3,
    STATE_WAIT_CRC,
    STATE_WAIT_EOF
} ParserState_t;

static ParserState_t g_state = STATE_WAIT_SOF;
static uint8_t       g_cmd_id = 0;
static uint8_t       g_pos_bytes[4];
static uint8_t       g_crc = 0;

void PacketParser_Init(void) {
    g_state = STATE_WAIT_SOF;
    g_cmd_id = 0;
    g_crc = 0;
}

int8_t PacketParser_FeedByte(uint8_t byte, int32_t *out_target_pos) {
    switch (g_state) {
        case STATE_WAIT_SOF:
            if (byte == WF_CMD_SOF) {
                g_state = STATE_WAIT_CMD;
            }
            return 0;

        case STATE_WAIT_CMD:
            g_cmd_id = byte;
            g_state = STATE_WAIT_POS_0;
            return 0;

        case STATE_WAIT_POS_0:
            g_pos_bytes[0] = byte;
            g_state = STATE_WAIT_POS_1;
            return 0;

        case STATE_WAIT_POS_1:
            g_pos_bytes[1] = byte;
            g_state = STATE_WAIT_POS_2;
            return 0;

        case STATE_WAIT_POS_2:
            g_pos_bytes[2] = byte;
            g_state = STATE_WAIT_POS_3;
            return 0;

        case STATE_WAIT_POS_3:
            g_pos_bytes[3] = byte;
            g_state = STATE_WAIT_CRC;
            return 0;

        case STATE_WAIT_CRC:
            g_crc = byte;
            g_state = STATE_WAIT_EOF;
            return 0;

        case STATE_WAIT_EOF: {
            g_state = STATE_WAIT_SOF;
            if (byte != WF_CMD_EOF) {
                return -1;
            }

            uint8_t crc_payload[5];
            crc_payload[0] = g_cmd_id;
            crc_payload[1] = g_pos_bytes[0];
            crc_payload[2] = g_pos_bytes[1];
            crc_payload[3] = g_pos_bytes[2];
            crc_payload[4] = g_pos_bytes[3];

            uint8_t calc_crc = CRC8_Calculate(crc_payload, 5);
            if (calc_crc != g_crc) {
                return -1;
            }

            if (g_cmd_id != WF_CMD_ID_SET_POS) {
                return -1;
            }

            if (out_target_pos != (int32_t*)0) {
                *out_target_pos = (int32_t)(
                    ((uint32_t)g_pos_bytes[0]) |
                    (((uint32_t)g_pos_bytes[1]) << 8) |
                    (((uint32_t)g_pos_bytes[2]) << 16) |
                    (((uint32_t)g_pos_bytes[3]) << 24)
                );
            }
            return 1;
        }

        default:
            g_state = STATE_WAIT_SOF;
            return 0;
    }
}

HAL_Status_t PacketParser_ParseBuffer(const uint8_t *buf, uint16_t len, int32_t *out_target_pos) {
    if (buf == (const uint8_t*)0 || len < WF_CMD_PACKET_LEN || out_target_pos == (int32_t*)0) {
        return HAL_ERROR;
    }

    if (buf[0] != WF_CMD_SOF || buf[7] != WF_CMD_EOF) {
        return HAL_ERROR;
    }

    uint8_t cmd_id = buf[1];
    if (cmd_id != WF_CMD_ID_SET_POS) {
        return HAL_ERROR;
    }

    uint8_t calc_crc = CRC8_Calculate(&buf[1], 5);
    if (calc_crc != buf[6]) {
        return HAL_ERROR;
    }

    *out_target_pos = (int32_t)(
        ((uint32_t)buf[2]) |
        (((uint32_t)buf[3]) << 8) |
        (((uint32_t)buf[4]) << 16) |
        (((uint32_t)buf[5]) << 24)
    );

    return HAL_OK;
}
