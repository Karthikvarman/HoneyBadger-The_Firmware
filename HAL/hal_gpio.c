#include "hal_gpio.h"
#include "../Config/wf_config.h"

static uint32_t GPIO_GetBase(char port) {

    (void)port;

    return 0;
}

HAL_Status_t HAL_GPIO_Init(const HAL_GPIO_Config_t *config) {
    if (config == NULL) {
        return HAL_ERROR;
    }

    uint32_t base = GPIO_GetBase(config->port);
    if (base == 0) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

void HAL_GPIO_Write(char port, uint8_t pin, HAL_PinState_t state) {
    uint32_t base = GPIO_GetBase(port);
    if (base == 0) return;

    (void)state;
    (void)pin;
}

HAL_PinState_t HAL_GPIO_Read(char port, uint8_t pin) {
    uint32_t base = GPIO_GetBase(port);
    if (base == 0) return HAL_PIN_LOW;

    (void)pin;
    return HAL_PIN_LOW;
}

void HAL_GPIO_Toggle(char port, uint8_t pin) {
    uint32_t base = GPIO_GetBase(port);
    if (base == 0) return;

    (void)pin;
}
