#include "hal_i2c.h"
#include "hal_gpio.h"
#include "../Config/wf_config.h"

HAL_Status_t HAL_I2C_Init(const HAL_I2C_Config_t *config) {
    if (config == NULL) return HAL_ERROR;

    return HAL_OK;
}

HAL_Status_t HAL_I2C_Read(HAL_I2C_Instance_t instance,
                           uint8_t slave_addr, uint8_t reg_addr,
                           uint8_t *data, uint16_t len,
                           uint32_t timeout_ms) {
    if (data == NULL || len == 0) return HAL_ERROR;

    (void)instance;
    (void)timeout_ms;

    (void)slave_addr;
    (void)reg_addr;
    (void)data;
    (void)len;

    return HAL_OK;
}

HAL_Status_t HAL_I2C_Write(HAL_I2C_Instance_t instance,
                            uint8_t slave_addr, uint8_t reg_addr,
                            const uint8_t *data, uint16_t len,
                            uint32_t timeout_ms) {
    if (data == NULL || len == 0) return HAL_ERROR;

    (void)instance;
    (void)timeout_ms;

    (void)slave_addr;
    (void)reg_addr;
    (void)data;
    (void)len;

    return HAL_OK;
}

HAL_Status_t HAL_I2C_BusRecovery(HAL_I2C_Instance_t instance) {
    (void)instance;

    return HAL_OK;
}
