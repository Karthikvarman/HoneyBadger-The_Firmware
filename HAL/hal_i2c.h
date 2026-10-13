#ifndef HAL_I2C_H
#define HAL_I2C_H

#include "hal_common.h"

typedef struct {
    HAL_I2C_Instance_t  instance;
    uint32_t            clock_hz;
} HAL_I2C_Config_t;

HAL_Status_t HAL_I2C_Init(const HAL_I2C_Config_t *config);

HAL_Status_t HAL_I2C_Read(HAL_I2C_Instance_t instance,
                           uint8_t slave_addr, uint8_t reg_addr,
                           uint8_t *data, uint16_t len,
                           uint32_t timeout_ms);

HAL_Status_t HAL_I2C_Write(HAL_I2C_Instance_t instance,
                            uint8_t slave_addr, uint8_t reg_addr,
                            const uint8_t *data, uint16_t len,
                            uint32_t timeout_ms);

HAL_Status_t HAL_I2C_BusRecovery(HAL_I2C_Instance_t instance);

#endif
