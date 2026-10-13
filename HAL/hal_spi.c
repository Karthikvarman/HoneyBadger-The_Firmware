#include "hal_spi.h"
#include "../Config/wf_config.h"

HAL_Status_t HAL_SPI_Init(const HAL_SPI_Config_t *config) {
    if (config == NULL) return HAL_ERROR;

    return HAL_OK;
}

HAL_Status_t HAL_SPI_TransmitReceive(HAL_SPI_Instance_t instance,
                                      const uint8_t *tx_data,
                                      uint8_t *rx_data,
                                      uint16_t len,
                                      uint32_t timeout_ms) {
    (void)instance;
    (void)timeout_ms;

    (void)tx_data;
    (void)rx_data;
    (void)len;

    return HAL_OK;
}

HAL_Status_t HAL_SPI_Transmit(HAL_SPI_Instance_t instance,
                               const uint8_t *data, uint16_t len,
                               uint32_t timeout_ms) {
    return HAL_SPI_TransmitReceive(instance, data, NULL, len, timeout_ms);
}

HAL_Status_t HAL_SPI_Receive(HAL_SPI_Instance_t instance,
                              uint8_t *data, uint16_t len,
                              uint32_t timeout_ms) {
    return HAL_SPI_TransmitReceive(instance, NULL, data, len, timeout_ms);
}
