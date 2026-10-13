cmake_minimum_required(VERSION 3.15)
project(HoneyBadger C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(
    Config
    HAL
    Middleware
    Application
    Core
)

set(SOURCES
    Core/main.c
    HAL/hal_gpio.c
    HAL/hal_uart.c
    HAL/hal_spi.c
    HAL/hal_i2c.c
    HAL/hal_timer.c
    Middleware/scheduler.c
    Middleware/bus_arbiter.c
    Middleware/crc.c
    Middleware/data_store.c
    Middleware/pid_controller.c
    Middleware/packet_builder.c
    Middleware/packet_parser.c
    Application/sensor_tasks.c
    Application/motor_control.c
    Application/telemetry.c
)

add_executable(HoneyBadger ${SOURCES})

set(TEST_SOURCES
    Tests/test_runner.c
    HAL/hal_timer.c
    HAL/hal_gpio.c
    HAL/hal_uart.c
    HAL/hal_spi.c
    HAL/hal_i2c.c
    Middleware/scheduler.c
    Middleware/bus_arbiter.c
    Middleware/crc.c
    Middleware/data_store.c
    Middleware/pid_controller.c
    Middleware/packet_builder.c
    Middleware/packet_parser.c
)

add_executable(HoneyBadger_Tests ${TEST_SOURCES})
