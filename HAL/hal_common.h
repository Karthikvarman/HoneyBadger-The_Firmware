#ifndef HAL_COMMON_H
#define HAL_COMMON_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    HAL_OK       = 0x00,
    HAL_ERROR    = 0x01,
    HAL_BUSY     = 0x02,
    HAL_TIMEOUT  = 0x03
} HAL_Status_t;

typedef enum {
    HAL_PIN_LOW  = 0,
    HAL_PIN_HIGH = 1
} HAL_PinState_t;

typedef enum {
    HAL_UART_1 = 1,
    HAL_UART_2 = 2,
    HAL_UART_3 = 3
} HAL_UART_Instance_t;

typedef enum {
    HAL_SPI_1 = 1,
    HAL_SPI_2 = 2
} HAL_SPI_Instance_t;

typedef enum {
    HAL_I2C_1 = 1
} HAL_I2C_Instance_t;

typedef enum {
    HAL_TIM_1 = 1,
    HAL_TIM_2 = 2,
    HAL_TIM_3 = 3,
    HAL_TIM_4 = 4
} HAL_TIM_Instance_t;

#define WF_CLAMP(val, min, max) \
    ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))

#define WF_ABS(x)  (((x) < 0) ? (-(x)) : (x))

#define WF_UNUSED(x)  ((void)(x))

#if defined(__GNUC__) || defined(__clang__)
#define WF_VOLATILE_READ(var)     (*(volatile __typeof__(var)*)&(var))
#define WF_VOLATILE_WRITE(var, v) (*(volatile __typeof__(var)*)&(var) = (v))
#else
#define WF_VOLATILE_READ(var)     (var)
#define WF_VOLATILE_WRITE(var, v) ((var) = (v))
#endif

#if defined(__ARMCC_VERSION) || (defined(__GNUC__) && (defined(__arm__) || defined(__aarch64__)))
#define WF_ENTER_CRITICAL()   __disable_irq()
#define WF_EXIT_CRITICAL()    __enable_irq()
#else
#define WF_ENTER_CRITICAL()   ((void)0)
#define WF_EXIT_CRITICAL()    ((void)0)
#endif

#endif
