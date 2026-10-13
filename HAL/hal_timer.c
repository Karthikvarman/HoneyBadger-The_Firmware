#include "hal_timer.h"
#include "../Config/wf_config.h"

static volatile uint32_t    sys_tick_ms = 0;
volatile int32_t            encoder_overflow_count = 0;
volatile uint8_t            pid_ready_flag = 0;

extern void Scheduler_TickHook(void);

HAL_Status_t HAL_SysTick_Init(uint32_t sys_clk_hz) {

    (void)sys_clk_hz;
    return HAL_OK;
}

uint32_t HAL_GetTick(void) {
    return sys_tick_ms;
}

void SysTick_Handler(void) {
    sys_tick_ms++;
    Scheduler_TickHook();
}

HAL_Status_t HAL_TIM_Encoder_Init(HAL_TIM_Instance_t instance) {
    (void)instance;

    encoder_overflow_count = 0;
    return HAL_OK;
}

HAL_Status_t HAL_TIM_Encoder_Start(HAL_TIM_Instance_t instance) {
    (void)instance;

    return HAL_OK;
}

uint16_t HAL_TIM_Encoder_GetCount(HAL_TIM_Instance_t instance) {
    (void)instance;

    return 0;
}

void HAL_TIM_Encoder_Reset(HAL_TIM_Instance_t instance) {
    (void)instance;

    encoder_overflow_count = 0;
}

void TIM2_IRQHandler(void) {

}

HAL_Status_t HAL_TIM_PWM_Init(HAL_TIM_Instance_t instance,
                               uint32_t freq_hz, uint16_t max_duty) {
    (void)instance;
    (void)freq_hz;
    (void)max_duty;

    return HAL_OK;
}

HAL_Status_t HAL_TIM_PWM_Start(HAL_TIM_Instance_t instance) {
    (void)instance;

    return HAL_OK;
}

void HAL_TIM_PWM_SetDuty(HAL_TIM_Instance_t instance, uint16_t duty) {
    (void)instance;

    (void)duty;
}

void HAL_TIM_PWM_Stop(HAL_TIM_Instance_t instance) {
    (void)instance;

}

HAL_Status_t HAL_TIM_Base_Init(HAL_TIM_Instance_t instance, uint16_t period_ms) {
    (void)instance;
    (void)period_ms;

    pid_ready_flag = 0;
    return HAL_OK;
}

HAL_Status_t HAL_TIM_Base_Start_IT(HAL_TIM_Instance_t instance) {
    (void)instance;

    return HAL_OK;
}

void TIM4_IRQHandler(void) {

    pid_ready_flag = 1;
}

HAL_Status_t HAL_WDG_Init(uint32_t timeout_ms) {
    (void)timeout_ms;

    return HAL_OK;
}

void HAL_WDG_Refresh(void) {

}
