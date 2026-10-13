#ifndef PACKET_PARSER_H
#define PACKET_PARSER_H

#include <stdint.h>
#include "../HAL/hal_common.h"

#define WF_CMD_SOF              0xBBU
#define WF_CMD_EOF              0x55U
#define WF_CMD_ID_SET_POS       0x01U
#define WF_CMD_PACKET_LEN       8U

void PacketParser_Init(void);

int8_t PacketParser_FeedByte(uint8_t byte, int32_t *out_target_pos);

HAL_Status_t PacketParser_ParseBuffer(const uint8_t *buf, uint16_t len, int32_t *out_target_pos);

#endif
