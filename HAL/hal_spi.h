#ifndef HAL_SPI_H
#define HAL_SPI_H

#include "hal_common.h"

typedef enum {
    HAL_SPI_MODE_0 = 0,
    HAL_SPI_MODE_1 = 1,
    HAL_SPI_MODE_2 = 2,
    HAL_SPI_MODE_3 = 3
} HAL_SPI_Mode_t;

typedef struct {
    HAL_SPI_Instance_t  instance;
    uint32_t            clock_hz;
    HAL_SPI_Mode_t      mode;
    uint8_t             data_bits;
} HAL_SPI_Config_t;

HAL_Status_t HAL_SPI_Init(const HAL_SPI_Config_t *config);

HAL_Status_t HAL_SPI_TransmitReceive(HAL_SPI_Instance_t instance,
                                      const uint8_t *tx_data,
                                      uint8_t *rx_data,
                                      uint16_t len,
                                      uint32_t timeout_ms);

HAL_Status_t HAL_SPI_Transmit(HAL_SPI_Instance_t instance,
                               const uint8_t *data, uint16_t len,
                               uint32_t timeout_ms);

HAL_Status_t HAL_SPI_Receive(HAL_SPI_Instance_t instance,
                              uint8_t *data, uint16_t len,
                              uint32_t timeout_ms);

#endif
