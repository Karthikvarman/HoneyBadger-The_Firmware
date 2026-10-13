#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include "hal_common.h"

typedef enum {
    HAL_GPIO_MODE_INPUT          = 0x00,
    HAL_GPIO_MODE_OUTPUT_PP      = 0x01,
    HAL_GPIO_MODE_OUTPUT_OD      = 0x02,
    HAL_GPIO_MODE_AF_PP          = 0x03,
    HAL_GPIO_MODE_AF_OD          = 0x04,
    HAL_GPIO_MODE_ANALOG         = 0x05
} HAL_GPIO_Mode_t;

typedef struct {
    char                port;
    uint8_t             pin;
    HAL_GPIO_Mode_t     mode;
} HAL_GPIO_Config_t;

HAL_Status_t HAL_GPIO_Init(const HAL_GPIO_Config_t *config);

void HAL_GPIO_Write(char port, uint8_t pin, HAL_PinState_t state);

HAL_PinState_t HAL_GPIO_Read(char port, uint8_t pin);

void HAL_GPIO_Toggle(char port, uint8_t pin);

#endif
