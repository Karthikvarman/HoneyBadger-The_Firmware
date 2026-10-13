#include "hal_uart.h"
#include "hal_gpio.h"

#define UART_INSTANCE_COUNT     3

static HAL_UART_RingBuf_t       uart_rx_buf[UART_INSTANCE_COUNT];
static HAL_UART_RxCallback_t    uart_rx_cb[UART_INSTANCE_COUNT];
static volatile uint8_t         uart_dma_busy[UART_INSTANCE_COUNT];

static inline uint8_t UART_Idx(HAL_UART_Instance_t inst) {
    return (uint8_t)(inst - 1);
}

static inline void RingBuf_Push(HAL_UART_RingBuf_t *rb, uint8_t byte) {
    if (rb->count < WF_UART_RX_BUF_SIZE) {
        rb->buffer[rb->head] = byte;
        rb->head = (rb->head + 1) % WF_UART_RX_BUF_SIZE;
        rb->count++;
    }

}

static inline uint8_t RingBuf_Pop(HAL_UART_RingBuf_t *rb) {
    uint8_t byte = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % WF_UART_RX_BUF_SIZE;
    rb->count--;
    return byte;
}

HAL_Status_t HAL_UART_Init(const HAL_UART_Config_t *config) {
    if (config == NULL) return HAL_ERROR;

    uint8_t idx = UART_Idx(config->instance);

    uart_rx_buf[idx].head  = 0;
    uart_rx_buf[idx].tail  = 0;
    uart_rx_buf[idx].count = 0;
    uart_rx_cb[idx]        = NULL;
    uart_dma_busy[idx]     = 0;

    return HAL_OK;
}

HAL_Status_t HAL_UART_Transmit(HAL_UART_Instance_t instance,
                                const uint8_t *data, uint16_t len,
                                uint32_t timeout_ms) {
    if (data == NULL || len == 0) return HAL_ERROR;

    (void)instance;
    (void)timeout_ms;

    return HAL_OK;
}

HAL_Status_t HAL_UART_Transmit_DMA(HAL_UART_Instance_t instance,
                                    const uint8_t *data, uint16_t len) {
    uint8_t idx = UART_Idx(instance);

    if (uart_dma_busy[idx]) return HAL_BUSY;
    if (data == NULL || len == 0) return HAL_ERROR;

    uart_dma_busy[idx] = 1;

    return HAL_OK;
}

uint16_t HAL_UART_Receive(HAL_UART_Instance_t instance,
                           uint8_t *data, uint16_t max_len) {
    uint8_t idx = UART_Idx(instance);
    HAL_UART_RingBuf_t *rb = &uart_rx_buf[idx];
    uint16_t bytes_read = 0;

    while (bytes_read < max_len && rb->count > 0) {
        WF_ENTER_CRITICAL();
        if (rb->count > 0) {
            data[bytes_read++] = RingBuf_Pop(rb);
        }
        WF_EXIT_CRITICAL();
    }

    return bytes_read;
}

uint16_t HAL_UART_RxAvailable(HAL_UART_Instance_t instance) {
    return uart_rx_buf[UART_Idx(instance)].count;
}

void HAL_UART_RegisterRxCallback(HAL_UART_Instance_t instance,
                                  HAL_UART_RxCallback_t callback) {
    uart_rx_cb[UART_Idx(instance)] = callback;
}

uint8_t HAL_UART_TxComplete(HAL_UART_Instance_t instance) {
    return !uart_dma_busy[UART_Idx(instance)];
}

static void UART_RX_IRQ_Common(uint8_t idx) {

    uint8_t byte = 0;
    RingBuf_Push(&uart_rx_buf[idx], byte);

    if (uart_rx_cb[idx]) {
        uart_rx_cb[idx]((HAL_UART_Instance_t)(idx + 1), byte);
    }
}

void UART1_RX_IRQHandler(void) {
    UART_RX_IRQ_Common(0);
}

void UART2_RX_IRQHandler(void) {
    UART_RX_IRQ_Common(1);
}

void UART3_RX_IRQHandler(void) {
    UART_RX_IRQ_Common(2);
}

void DMA_UART3_TX_IRQHandler(void) {

    uart_dma_busy[UART_Idx(HAL_UART_3)] = 0;
}
