#ifndef HB_CONFIG_H
#define HB_CONFIG_H

#include <stdint.h>

#define WF_SYSCLK_HZ           72000000UL
#define WF_APB1_HZ             36000000UL
#define WF_APB2_HZ             72000000UL

#define WF_SYSTICK_PERIOD_MS    1U

#define WF_UART1_BAUD           115200U
#define WF_UART1_TX_PIN_PORT    'A'
#define WF_UART1_TX_PIN_NUM     9
#define WF_UART1_RX_PIN_PORT    'A'
#define WF_UART1_RX_PIN_NUM     10

#define WF_UART2_BAUD           115200U
#define WF_UART2_TX_PIN_PORT    'A'
#define WF_UART2_TX_PIN_NUM     2
#define WF_UART2_RX_PIN_PORT    'A'
#define WF_UART2_RX_PIN_NUM     3

#define WF_UART3_BAUD           115200U
#define WF_UART3_TX_PIN_PORT    'B'
#define WF_UART3_TX_PIN_NUM     10
#define WF_UART3_RX_PIN_PORT    'B'
#define WF_UART3_RX_PIN_NUM     11

#define WF_UART_RX_BUF_SIZE     64U
#define WF_UART_TX_BUF_SIZE     128U

#define WF_SPI1_CLK_HZ          1000000UL
#define WF_SPI1_MOSI_PORT       'A'
#define WF_SPI1_MOSI_PIN        7
#define WF_SPI1_MISO_PORT       'A'
#define WF_SPI1_MISO_PIN        6
#define WF_SPI1_SCK_PORT        'A'
#define WF_SPI1_SCK_PIN         5

#define WF_CS3_PORT             'B'
#define WF_CS3_PIN              0
#define WF_CS4_PORT             'B'
#define WF_CS4_PIN              1

#define WF_SPI2_CLK_HZ          1000000UL
#define WF_SPI2_MOSI_PORT       'B'
#define WF_SPI2_MOSI_PIN        15
#define WF_SPI2_MISO_PORT       'B'
#define WF_SPI2_MISO_PIN        14
#define WF_SPI2_SCK_PORT        'B'
#define WF_SPI2_SCK_PIN         13

#define WF_CS5_PORT             'B'
#define WF_CS5_PIN              12
#define WF_CS6_PORT             'A'
#define WF_CS6_PIN              8

#define WF_I2C1_SPEED_HZ        400000UL
#define WF_I2C1_SDA_PORT        'B'
#define WF_I2C1_SDA_PIN         7
#define WF_I2C1_SCL_PORT        'B'
#define WF_I2C1_SCL_PIN         6

#define WF_I2C_RECOVERY_CLK_CYCLES  9U

#define WF_ENC_TIM              2
#define WF_ENC_CHA_PORT         'A'
#define WF_ENC_CHA_PIN          0
#define WF_ENC_CHB_PORT         'A'
#define WF_ENC_CHB_PIN          1
#define WF_ENC_COUNTS_PER_REV   1000U

#define WF_PWM_TIM              3
#define WF_PWM_FREQ_HZ          20000UL
#define WF_PWM_PIN_PORT         'A'
#define WF_PWM_PIN_NUM          6
#define WF_PWM_MAX              999U

#define WF_PID_TIM              4
#define WF_PID_PERIOD_MS        10U

#define WF_MOTOR_DIR_A_PORT     'C'
#define WF_MOTOR_DIR_A_PIN      0
#define WF_MOTOR_DIR_B_PORT     'C'
#define WF_MOTOR_DIR_B_PIN      1

#define WF_WDG_TIMEOUT_MS       2000U

#define WF_DMA_UART3_TX_CH      2U

#define WF_IRQ_PRIO_ENCODER     0U
#define WF_IRQ_PRIO_PID_TIM     1U
#define WF_IRQ_PRIO_SYSTICK     2U
#define WF_IRQ_PRIO_UART3_RX    3U
#define WF_IRQ_PRIO_UART1_RX    4U
#define WF_IRQ_PRIO_UART2_RX    5U
#define WF_IRQ_PRIO_DMA         6U
#define WF_IRQ_PRIO_I2C         7U

#endif
