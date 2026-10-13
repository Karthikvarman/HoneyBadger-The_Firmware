#ifndef BUS_ARBITER_H
#define BUS_ARBITER_H

#include <stdint.h>
#include "../HAL/hal_common.h"

typedef enum {
    WF_BUS_SPI1 = 0,
    WF_BUS_SPI2 = 1,
    WF_BUS_I2C1 = 2,
    WF_BUS_COUNT = 3
} WF_BusID_t;

void BusArbiter_Init(void);

HAL_Status_t BusArbiter_Lock(WF_BusID_t bus_id, uint8_t owner_id);

HAL_Status_t BusArbiter_Unlock(WF_BusID_t bus_id, uint8_t owner_id);

uint8_t BusArbiter_IsLocked(WF_BusID_t bus_id);

uint8_t BusArbiter_GetOwner(WF_BusID_t bus_id);

#endif
