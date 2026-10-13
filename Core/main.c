#include <stdint.h>
#include "../Config/wf_config.h"
#include "../Config/sensor_config.h"
#include "../Config/pid_config.h"
#include "../HAL/hal_common.h"
#include "../HAL/hal_gpio.h"
#include "../HAL/hal_uart.h"
#include "../HAL/hal_spi.h"
#include "../HAL/hal_i2c.h"
#include "../HAL/hal_timer.h"
#include "../Middleware/scheduler.h"
#include "../Middleware/bus_arbiter.h"
#include "../Middleware/data_store.h"
#include "../Application/sensor_tasks.h"
#include "../Application/motor_control.h"
#include "../Application/telemetry.h"

extern volatile uint8_t pid_ready_flag;

static void System_GPIO_Init(void) {
    HAL_GPIO_Config_t pin_cfg;

    pin_cfg.port = WF_MOTOR_DIR_A_PORT;
    pin_cfg.pin  = WF_MOTOR_DIR_A_PIN;
    pin_cfg.mode = HAL_GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(&pin_cfg);

    pin_cfg.port = WF_MOTOR_DIR_B_PORT;
    pin_cfg.pin  = WF_MOTOR_DIR_B_PIN;
    pin_cfg.mode = HAL_GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(&pin_cfg);

    pin_cfg.mode = HAL_GPIO_MODE_OUTPUT_PP;

    pin_cfg.port = WF_CS3_PORT; pin_cfg.pin = WF_CS3_PIN;
    HAL_GPIO_Init(&pin_cfg);

    pin_cfg.port = WF_CS4_PORT; pin_cfg.pin = WF_CS4_PIN;
    HAL_GPIO_Init(&pin_cfg);

    pin_cfg.port = WF_CS5_PORT; pin_cfg.pin = WF_CS5_PIN;
    HAL_GPIO_Init(&pin_cfg);

    pin_cfg.port = WF_CS6_PORT; pin_cfg.pin = WF_CS6_PIN;
    HAL_GPIO_Init(&pin_cfg);

    HAL_GPIO_Write(WF_CS3_PORT, WF_CS3_PIN, HAL_PIN_HIGH);
    HAL_GPIO_Write(WF_CS4_PORT, WF_CS4_PIN, HAL_PIN_HIGH);
    HAL_GPIO_Write(WF_CS5_PORT, WF_CS5_PIN, HAL_PIN_HIGH);
    HAL_GPIO_Write(WF_CS6_PORT, WF_CS6_PIN, HAL_PIN_HIGH);
}

static void System_Peripherals_Init(void) {

    HAL_UART_Config_t u1_cfg = {
        .instance = HAL_UART_1,
        .baud_rate = WF_UART1_BAUD,
        .enable_rx_irq = 1,
        .enable_dma_tx = 0
    };
    HAL_UART_Init(&u1_cfg);

    HAL_UART_Config_t u2_cfg = {
        .instance = HAL_UART_2,
        .baud_rate = WF_UART2_BAUD,
        .enable_rx_irq = 1,
        .enable_dma_tx = 0
    };
    HAL_UART_Init(&u2_cfg);

    HAL_UART_Config_t u3_cfg = {
        .instance = HAL_UART_3,
        .baud_rate = WF_UART3_BAUD,
        .enable_rx_irq = 1,
        .enable_dma_tx = 1
    };
    HAL_UART_Init(&u3_cfg);

    HAL_SPI_Config_t spi1_cfg = {
        .instance = HAL_SPI_1,
        .clock_hz = WF_SPI1_CLK_HZ,
        .mode = HAL_SPI_MODE_0,
        .data_bits = 8
    };
    HAL_SPI_Init(&spi1_cfg);

    HAL_SPI_Config_t spi2_cfg = {
        .instance = HAL_SPI_2,
        .clock_hz = WF_SPI2_CLK_HZ,
        .mode = HAL_SPI_MODE_0,
        .data_bits = 8
    };
    HAL_SPI_Init(&spi2_cfg);

    HAL_I2C_Config_t i2c_cfg = {
        .instance = HAL_I2C_1,
        .clock_hz = WF_I2C1_SPEED_HZ
    };
    HAL_I2C_Init(&i2c_cfg);

    HAL_TIM_Encoder_Init(HAL_TIM_2);
    HAL_TIM_PWM_Init(HAL_TIM_3, WF_PWM_FREQ_HZ, WF_PWM_MAX);
    HAL_TIM_Base_Init(HAL_TIM_4, WF_PID_PERIOD_MS);
}

static void System_Scheduler_Setup(void) {
    Scheduler_Init();

    Scheduler_AddTask("UART3_TX",     WF_TELEMETRY_PERIOD_MS, WF_TELEMETRY_PRIORITY, Task_Telemetry_TX);
    Scheduler_AddTask("S1_UART1",     WF_S1_PERIOD_MS,        WF_S1_PRIORITY,        Task_Read_S1);
    Scheduler_AddTask("S2_UART2",     WF_S2_PERIOD_MS,        WF_S2_PRIORITY,        Task_Read_S2);
    Scheduler_AddTask("S3_SPI1",      WF_S3_PERIOD_MS,        WF_S3_PRIORITY,        Task_Read_S3);
    Scheduler_AddTask("S4_SPI1",      WF_S4_PERIOD_MS,        WF_S4_PRIORITY,        Task_Read_S4);
    Scheduler_AddTask("S5_SPI2",      WF_S5_PERIOD_MS,        WF_S5_PRIORITY,        Task_Read_S5);
    Scheduler_AddTask("S6_SPI2",      WF_S6_PERIOD_MS,        WF_S6_PRIORITY,        Task_Read_S6);
    Scheduler_AddTask("I2C_S7toS10",  WF_I2C_PERIOD_MS,       WF_I2C_PRIORITY,       Task_Read_I2C_All);
}

int main(void) {

    System_GPIO_Init();
    System_Peripherals_Init();

    BusArbiter_Init();
    DataStore_Init();

    SensorTasks_Init();
    MotorControl_Init();
    Telemetry_Init();

    System_Scheduler_Setup();

    HAL_WDG_Init(WF_WDG_TIMEOUT_MS);

    HAL_SysTick_Init(WF_SYSCLK_HZ);
    HAL_TIM_Base_Start_IT(HAL_TIM_4);

    while (1) {

        if (pid_ready_flag) {
            pid_ready_flag = 0;
            MotorControl_ExecutePID();
        }

        Telemetry_ProcessRX();

        Scheduler_Dispatch();

        HAL_WDG_Refresh();
    }

    return 0;
}
