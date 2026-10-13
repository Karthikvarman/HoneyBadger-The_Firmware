#include "data_store.h"
#include <string.h>

static WF_DataSnapshot_t buffer_pair[2];
static volatile uint8_t  write_index = 0;

void DataStore_Init(void) {
    write_index = 0;

    for (uint8_t b = 0; b < 2; b++) {
        buffer_pair[b].snapshot_timestamp_ms = 0;
        for (uint8_t i = 0; i < WF_SENSOR_COUNT; i++) {
            buffer_pair[b].sensors[i].id           = (WF_SensorID_t)(i + 1);
            buffer_pair[b].sensors[i].status       = WF_SENSOR_NOT_READY;
            buffer_pair[b].sensors[i].data_len     = 0;
            buffer_pair[b].sensors[i].timestamp_ms = 0;
            memset(buffer_pair[b].sensors[i].data, 0, WF_SENSOR_MAX_DATA_LEN);
        }
    }
}

HAL_Status_t DataStore_WriteSensor(WF_SensorID_t id,
                                   WF_SensorStatus_t status,
                                   const uint8_t *data,
                                   uint8_t len,
                                   uint32_t timestamp_ms) {
    if (id < WF_SENSOR_S1 || id > WF_SENSOR_S10) {
        return HAL_ERROR;
    }

    if (len > WF_SENSOR_MAX_DATA_LEN) {
        len = WF_SENSOR_MAX_DATA_LEN;
    }

    uint8_t idx = (uint8_t)(id - 1);
    uint8_t active = write_index;

    WF_SensorData_t *entry = &buffer_pair[active].sensors[idx];
    entry->id = id;
    entry->status = status;
    entry->data_len = len;
    entry->timestamp_ms = timestamp_ms;

    if (data != (const uint8_t*)0 && len > 0) {
        memcpy(entry->data, data, len);
    } else {
        memset(entry->data, 0, sizeof(entry->data));
    }

    return HAL_OK;
}

void DataStore_SwapBuffers(void) {

    uint8_t current = write_index;
    uint8_t next = (current == 0) ? 1 : 0;

    memcpy(&buffer_pair[next], &buffer_pair[current], sizeof(WF_DataSnapshot_t));

    write_index = next;
}

const WF_SensorData_t* DataStore_GetSnapshotSensor(WF_SensorID_t id) {
    if (id < WF_SENSOR_S1 || id > WF_SENSOR_S10) {
        return (const WF_SensorData_t*)0;
    }

    uint8_t read_index = (write_index == 0) ? 1 : 0;
    uint8_t idx = (uint8_t)(id - 1);

    return &buffer_pair[read_index].sensors[idx];
}

uint32_t DataStore_GetSnapshotTimestamp(void) {
    uint8_t read_index = (write_index == 0) ? 1 : 0;
    return buffer_pair[read_index].snapshot_timestamp_ms;
}
