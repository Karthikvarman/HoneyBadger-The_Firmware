#include "sensor_tasks.h"
#include "../Config/wf_config.h"
#include "../Config/sensor_config.h"
#include "../HAL/hal_uart.h"
#include "../HAL/hal_spi.h"
#include "../HAL/hal_i2c.h"
#include "../HAL/hal_gpio.h"
#include "../HAL/hal_timer.h"
#include "../Middleware/bus_arbiter.h"
#include "../Middleware/data_store.h"

const WF_I2CSensorEntry_t wf_i2c_sensor_table[WF_I2C_SENSOR_COUNT] = {
    { WF_SENSOR_S7,  WF_S7_I2C_ADDR,  WF_S7_DATA_LEN,  WF_I2C_DATA_REG },
    { WF_SENSOR_S8,  WF_S8_I2C_ADDR,  WF_S8_DATA_LEN,  WF_I2C_DATA_REG },
    { WF_SENSOR_S9,  WF_S9_I2C_ADDR,  WF_S9_DATA_LEN,  WF_I2C_DATA_REG },
    { WF_SENSOR_S10, WF_S10_I2C_ADDR, WF_S10_DATA_LEN, WF_I2C_DATA_REG }
};

void SensorTasks_Init(void) {

    HAL_GPIO_Write(WF_CS3_PORT, WF_CS3_PIN, HAL_PIN_HIGH);
    HAL_GPIO_Write(WF_CS4_PORT, WF_CS4_PIN, HAL_PIN_HIGH);
    HAL_GPIO_Write(WF_CS5_PORT, WF_CS5_PIN, HAL_PIN_HIGH);
    HAL_GPIO_Write(WF_CS6_PORT, WF_CS6_PIN, HAL_PIN_HIGH);
}

void Task_Read_S1(void) {
    uint8_t cmd = WF_UART_READ_CMD;
    uint8_t rx_buf[WF_S1_DATA_LEN];
    uint32_t now = HAL_GetTick();

    HAL_UART_Transmit(HAL_UART_1, &cmd, 1, 10);

    uint16_t read_bytes = HAL_UART_Receive(HAL_UART_1, rx_buf, WF_S1_DATA_LEN);

    if (read_bytes == WF_S1_DATA_LEN) {
        DataStore_WriteSensor(WF_SENSOR_S1, WF_SENSOR_OK, rx_buf, WF_S1_DATA_LEN, now);
    } else {
        DataStore_WriteSensor(WF_SENSOR_S1, WF_SENSOR_TIMEOUT, (const uint8_t*)0, 0, now);
    }
}

void Task_Read_S2(void) {
    uint8_t cmd = WF_UART_READ_CMD;
    uint8_t rx_buf[WF_S2_DATA_LEN];
    uint32_t now = HAL_GetTick();

    HAL_UART_Transmit(HAL_UART_2, &cmd, 1, 10);

    uint16_t read_bytes = HAL_UART_Receive(HAL_UART_2, rx_buf, WF_S2_DATA_LEN);

    if (read_bytes == WF_S2_DATA_LEN) {
        DataStore_WriteSensor(WF_SENSOR_S2, WF_SENSOR_OK, rx_buf, WF_S2_DATA_LEN, now);
    } else {
        DataStore_WriteSensor(WF_SENSOR_S2, WF_SENSOR_TIMEOUT, (const uint8_t*)0, 0, now);
    }
}

void Task_Read_S3(void) {
    uint32_t now = HAL_GetTick();

    if (BusArbiter_Lock(WF_BUS_SPI1, (uint8_t)WF_SENSOR_S3) != HAL_OK) {
        DataStore_WriteSensor(WF_SENSOR_S3, WF_SENSOR_BUS_ERROR, (const uint8_t*)0, 0, now);
        return;
    }

    uint8_t tx_buf[1 + WF_S3_DATA_LEN] = { WF_SPI_READ_CMD, 0x00, 0x00, 0x00, 0x00 };
    uint8_t rx_buf[1 + WF_S3_DATA_LEN] = { 0 };

    HAL_GPIO_Write(WF_CS3_PORT, WF_CS3_PIN, HAL_PIN_LOW);

    HAL_Status_t status = HAL_SPI_TransmitReceive(HAL_SPI_1, tx_buf, rx_buf,
                                                  sizeof(tx_buf), WF_SPI_SENSOR_TIMEOUT_MS);

    HAL_GPIO_Write(WF_CS3_PORT, WF_CS3_PIN, HAL_PIN_HIGH);

    BusArbiter_Unlock(WF_BUS_SPI1, (uint8_t)WF_SENSOR_S3);

    if (status == HAL_OK) {

        DataStore_WriteSensor(WF_SENSOR_S3, WF_SENSOR_OK, &rx_buf[1], WF_S3_DATA_LEN, now);
    } else {
        DataStore_WriteSensor(WF_SENSOR_S3, WF_SENSOR_TIMEOUT, (const uint8_t*)0, 0, now);
    }
}

void Task_Read_S4(void) {
    uint32_t now = HAL_GetTick();

    if (BusArbiter_Lock(WF_BUS_SPI1, (uint8_t)WF_SENSOR_S4) != HAL_OK) {
        DataStore_WriteSensor(WF_SENSOR_S4, WF_SENSOR_BUS_ERROR, (const uint8_t*)0, 0, now);
        return;
    }

    uint8_t tx_buf[1 + WF_S4_DATA_LEN] = { WF_SPI_READ_CMD, 0x00, 0x00 };
    uint8_t rx_buf[1 + WF_S4_DATA_LEN] = { 0 };

    HAL_GPIO_Write(WF_CS4_PORT, WF_CS4_PIN, HAL_PIN_LOW);

    HAL_Status_t status = HAL_SPI_TransmitReceive(HAL_SPI_1, tx_buf, rx_buf,
                                                  sizeof(tx_buf), WF_SPI_SENSOR_TIMEOUT_MS);

    HAL_GPIO_Write(WF_CS4_PORT, WF_CS4_PIN, HAL_PIN_HIGH);

    BusArbiter_Unlock(WF_BUS_SPI1, (uint8_t)WF_SENSOR_S4);

    if (status == HAL_OK) {
        DataStore_WriteSensor(WF_SENSOR_S4, WF_SENSOR_OK, &rx_buf[1], WF_S4_DATA_LEN, now);
    } else {
        DataStore_WriteSensor(WF_SENSOR_S4, WF_SENSOR_TIMEOUT, (const uint8_t*)0, 0, now);
    }
}

void Task_Read_S5(void) {
    uint32_t now = HAL_GetTick();

    if (BusArbiter_Lock(WF_BUS_SPI2, (uint8_t)WF_SENSOR_S5) != HAL_OK) {
        DataStore_WriteSensor(WF_SENSOR_S5, WF_SENSOR_BUS_ERROR, (const uint8_t*)0, 0, now);
        return;
    }

    uint8_t tx_buf[1 + WF_S5_DATA_LEN] = { WF_SPI_READ_CMD, 0x00, 0x00 };
    uint8_t rx_buf[1 + WF_S5_DATA_LEN] = { 0 };

    HAL_GPIO_Write(WF_CS5_PORT, WF_CS5_PIN, HAL_PIN_LOW);

    HAL_Status_t status = HAL_SPI_TransmitReceive(HAL_SPI_2, tx_buf, rx_buf,
                                                  sizeof(tx_buf), WF_SPI_SENSOR_TIMEOUT_MS);

    HAL_GPIO_Write(WF_CS5_PORT, WF_CS5_PIN, HAL_PIN_HIGH);

    BusArbiter_Unlock(WF_BUS_SPI2, (uint8_t)WF_SENSOR_S5);

    if (status == HAL_OK) {
        DataStore_WriteSensor(WF_SENSOR_S5, WF_SENSOR_OK, &rx_buf[1], WF_S5_DATA_LEN, now);
    } else {
        DataStore_WriteSensor(WF_SENSOR_S5, WF_SENSOR_TIMEOUT, (const uint8_t*)0, 0, now);
    }
}

void Task_Read_S6(void) {
    uint32_t now = HAL_GetTick();

    if (BusArbiter_Lock(WF_BUS_SPI2, (uint8_t)WF_SENSOR_S6) != HAL_OK) {
        DataStore_WriteSensor(WF_SENSOR_S6, WF_SENSOR_BUS_ERROR, (const uint8_t*)0, 0, now);
        return;
    }

    uint8_t tx_buf[1 + WF_S6_DATA_LEN] = { WF_SPI_READ_CMD, 0x00, 0x00 };
    uint8_t rx_buf[1 + WF_S6_DATA_LEN] = { 0 };

    HAL_GPIO_Write(WF_CS6_PORT, WF_CS6_PIN, HAL_PIN_LOW);

    HAL_Status_t status = HAL_SPI_TransmitReceive(HAL_SPI_2, tx_buf, rx_buf,
                                                  sizeof(tx_buf), WF_SPI_SENSOR_TIMEOUT_MS);

    HAL_GPIO_Write(WF_CS6_PORT, WF_CS6_PIN, HAL_PIN_HIGH);

    BusArbiter_Unlock(WF_BUS_SPI2, (uint8_t)WF_SENSOR_S6);

    if (status == HAL_OK) {
        DataStore_WriteSensor(WF_SENSOR_S6, WF_SENSOR_OK, &rx_buf[1], WF_S6_DATA_LEN, now);
    } else {
        DataStore_WriteSensor(WF_SENSOR_S6, WF_SENSOR_TIMEOUT, (const uint8_t*)0, 0, now);
    }
}

void Task_Read_I2C_All(void) {
    uint32_t now = HAL_GetTick();

    if (BusArbiter_Lock(WF_BUS_I2C1, 0x70) != HAL_OK) {
        for (uint8_t i = 0; i < WF_I2C_SENSOR_COUNT; i++) {
            DataStore_WriteSensor(wf_i2c_sensor_table[i].id, WF_SENSOR_BUS_ERROR,
                                  (const uint8_t*)0, 0, now);
        }
        return;
    }

    for (uint8_t i = 0; i < WF_I2C_SENSOR_COUNT; i++) {
        const WF_I2CSensorEntry_t *entry = &wf_i2c_sensor_table[i];
        uint8_t data_buf[WF_SENSOR_MAX_DATA_LEN];

        HAL_Status_t status = HAL_I2C_Read(HAL_I2C_1, entry->i2c_addr, entry->reg_addr,
                                           data_buf, entry->data_len, WF_I2C_SENSOR_TIMEOUT_MS);

        if (status == HAL_OK) {
            DataStore_WriteSensor(entry->id, WF_SENSOR_OK, data_buf, entry->data_len, now);
        } else {

            HAL_I2C_BusRecovery(HAL_I2C_1);
            DataStore_WriteSensor(entry->id, WF_SENSOR_TIMEOUT, (const uint8_t*)0, 0, now);
        }
    }

    BusArbiter_Unlock(WF_BUS_I2C1, 0x70);
}
