#include "bus_arbiter.h"

typedef struct {
    volatile uint8_t is_locked;
    volatile uint8_t owner_id;
} BusLock_t;

static BusLock_t bus_locks[WF_BUS_COUNT];

void BusArbiter_Init(void) {
    for (uint8_t i = 0; i < WF_BUS_COUNT; i++) {
        bus_locks[i].is_locked = 0;
        bus_locks[i].owner_id = 0;
    }
}

HAL_Status_t BusArbiter_Lock(WF_BusID_t bus_id, uint8_t owner_id) {
    if (bus_id >= WF_BUS_COUNT || owner_id == 0) {
        return HAL_ERROR;
    }

    if (bus_locks[bus_id].is_locked) {
        if (bus_locks[bus_id].owner_id == owner_id) {
            return HAL_OK;
        }
        return HAL_BUSY;
    }

    bus_locks[bus_id].is_locked = 1;
    bus_locks[bus_id].owner_id = owner_id;
    return HAL_OK;
}

HAL_Status_t BusArbiter_Unlock(WF_BusID_t bus_id, uint8_t owner_id) {
    if (bus_id >= WF_BUS_COUNT) {
        return HAL_ERROR;
    }

    if (!bus_locks[bus_id].is_locked) {
        return HAL_OK;
    }

    if (bus_locks[bus_id].owner_id != owner_id) {
        return HAL_ERROR;
    }

    bus_locks[bus_id].is_locked = 0;
    bus_locks[bus_id].owner_id = 0;
    return HAL_OK;
}

uint8_t BusArbiter_IsLocked(WF_BusID_t bus_id) {
    if (bus_id >= WF_BUS_COUNT) {
        return 0;
    }
    return bus_locks[bus_id].is_locked;
}

uint8_t BusArbiter_GetOwner(WF_BusID_t bus_id) {
    if (bus_id >= WF_BUS_COUNT) {
        return 0;
    }
    return bus_locks[bus_id].owner_id;
}
