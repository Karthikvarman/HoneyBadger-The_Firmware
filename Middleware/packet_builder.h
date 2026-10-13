#ifndef PACKET_BUILDER_H
#define PACKET_BUILDER_H

#include <stdint.h>
#include "../HAL/hal_common.h"
#include "data_store.h"

#define WF_PKT_SOF              0xAAU
#define WF_PKT_EOF              0x55U
#define WF_PKT_ID_TELEMETRY     0x01U

#define WF_TELEMETRY_PKT_MAX    128U

void PacketBuilder_Init(void);

HAL_Status_t PacketBuilder_BuildTelemetry(uint8_t *out_buf,
                                          uint16_t max_buf_len,
                                          uint16_t *out_pkt_len);

uint8_t PacketBuilder_GetSeqNum(void);

#endif
