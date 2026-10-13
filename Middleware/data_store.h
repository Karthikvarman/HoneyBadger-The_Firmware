#ifndef DATA_STORE_H
#define DATA_STORE_H

#include <stdint.h>
#include "../Config/sensor_config.h"
#include "../HAL/hal_common.h"

typedef struct {
    WF_SensorData_t sensors[WF_SENSOR_COUNT];
    uint32_t        snapshot_timestamp_ms;
} WF_DataSnapshot_t;

void DataStore_Init(void);

HAL_Status_t DataStore_WriteSensor(WF_SensorID_t id,
                                   WF_SensorStatus_t status,
                                   const uint8_t *data,
                                   uint8_t len,
                                   uint32_t timestamp_ms);

void DataStore_SwapBuffers(void);

const WF_SensorData_t* DataStore_GetSnapshotSensor(WF_SensorID_t id);

uint32_t DataStore_GetSnapshotTimestamp(void);

#endif
