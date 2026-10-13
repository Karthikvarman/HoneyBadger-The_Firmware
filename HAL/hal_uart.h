#ifndef HAL_UART_H
#define HAL_UART_H

#include "hal_common.h"
#include "../Config/wf_config.h"

typedef struct {
    HAL_UART_Instance_t instance;
    uint32_t            baud_rate;
    uint8_t             enable_rx_irq;
    uint8_t             enable_dma_tx;
} HAL_UART_Config_t;

typedef struct {
    uint8_t     buffer[WF_UART_RX_BUF_SIZE];
    volatile uint16_t   head;
    volatile uint16_t   tail;
    volatile uint16_t   count;
} HAL_UART_RingBuf_t;

typedef void (*HAL_UART_RxCallback_t)(HAL_UART_Instance_t instance, uint8_t byte);

HAL_Status_t HAL_UART_Init(const HAL_UART_Config_t *config);

HAL_Status_t HAL_UART_Transmit(HAL_UART_Instance_t instance,
                                const uint8_t *data, uint16_t len,
                                uint32_t timeout_ms);

HAL_Status_t HAL_UART_Transmit_DMA(HAL_UART_Instance_t instance,
                                    const uint8_t *data, uint16_t len);

uint16_t HAL_UART_Receive(HAL_UART_Instance_t instance,
                           uint8_t *data, uint16_t max_len);

uint16_t HAL_UART_RxAvailable(HAL_UART_Instance_t instance);

void HAL_UART_RegisterRxCallback(HAL_UART_Instance_t instance,
                                  HAL_UART_RxCallback_t callback);

uint8_t HAL_UART_TxComplete(HAL_UART_Instance_t instance);

void UART1_RX_IRQHandler(void);
void UART2_RX_IRQHandler(void);
void UART3_RX_IRQHandler(void);
void DMA_UART3_TX_IRQHandler(void);

#endif
