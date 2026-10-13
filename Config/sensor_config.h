#ifndef SENSOR_CONFIG_H
#define SENSOR_CONFIG_H

#include <stdint.h>

typedef enum {
    WF_SENSOR_S1   = 0x01,
    WF_SENSOR_S2   = 0x02,
    WF_SENSOR_S3   = 0x03,
    WF_SENSOR_S4   = 0x04,
    WF_SENSOR_S5   = 0x05,
    WF_SENSOR_S6   = 0x06,
    WF_SENSOR_S7   = 0x07,
    WF_SENSOR_S8   = 0x08,
    WF_SENSOR_S9   = 0x09,
    WF_SENSOR_S10  = 0x0A,
    WF_SENSOR_COUNT = 10
} WF_SensorID_t;

#define WF_S1_PERIOD_MS         500U
#define WF_S2_PERIOD_MS         250U
#define WF_S3_PERIOD_MS         10U
#define WF_S4_PERIOD_MS         1000U
#define WF_S5_PERIOD_MS         800U
#define WF_S6_PERIOD_MS         1500U
#define WF_I2C_PERIOD_MS        450U

#define WF_TELEMETRY_PERIOD_MS  500U

#define WF_S1_PRIORITY          1U
#define WF_S2_PRIORITY          2U
#define WF_S3_PRIORITY          3U
#define WF_S4_PRIORITY          4U
#define WF_S5_PRIORITY          5U
#define WF_S6_PRIORITY          6U
#define WF_I2C_PRIORITY         7U
#define WF_TELEMETRY_PRIORITY   0U

#define WF_UART_READ_CMD        0x52
#define WF_UART_READ_CMD_LEN    1U

#define WF_S1_DATA_LEN          2U
#define WF_S2_DATA_LEN          2U

#define WF_S3_DATA_LEN          4U
#define WF_S4_DATA_LEN          2U
#define WF_S5_DATA_LEN          2U
#define WF_S6_DATA_LEN          2U

#define WF_SPI_READ_CMD         0x80

#define WF_S7_I2C_ADDR          0x48
#define WF_S8_I2C_ADDR          0x49
#define WF_S9_I2C_ADDR          0x4A
#define WF_S10_I2C_ADDR         0x4B

#define WF_I2C_DATA_REG         0x00

#define WF_S7_DATA_LEN          2U
#define WF_S8_DATA_LEN          2U
#define WF_S9_DATA_LEN          2U
#define WF_S10_DATA_LEN         2U

#define WF_UART_SENSOR_TIMEOUT_MS   50U
#define WF_SPI_SENSOR_TIMEOUT_MS    5U
#define WF_I2C_SENSOR_TIMEOUT_MS    10U

#define WF_SENSOR_MAX_DATA_LEN  8U

typedef enum {
    WF_SENSOR_OK        = 0x00,
    WF_SENSOR_TIMEOUT   = 0x01,
    WF_SENSOR_CRC_ERR   = 0x02,
    WF_SENSOR_NOT_READY = 0x03,
    WF_SENSOR_BUS_ERROR = 0x04
} WF_SensorStatus_t;

typedef struct {
    WF_SensorID_t       id;
    WF_SensorStatus_t   status;
    uint8_t             data_len;
    uint8_t             data[WF_SENSOR_MAX_DATA_LEN];
    uint32_t            timestamp_ms;
} WF_SensorData_t;

typedef struct {
    WF_SensorID_t       id;
    uint8_t             i2c_addr;
    uint8_t             data_len;
    uint8_t             reg_addr;
} WF_I2CSensorEntry_t;

#define WF_I2C_SENSOR_COUNT     4U

extern const WF_I2CSensorEntry_t wf_i2c_sensor_table[WF_I2C_SENSOR_COUNT];

#endif
