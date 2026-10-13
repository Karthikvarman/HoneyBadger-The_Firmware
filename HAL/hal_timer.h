#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include "hal_common.h"

HAL_Status_t HAL_SysTick_Init(uint32_t sys_clk_hz);

uint32_t HAL_GetTick(void);

void SysTick_Handler(void);

HAL_Status_t HAL_TIM_Encoder_Init(HAL_TIM_Instance_t instance);

HAL_Status_t HAL_TIM_Encoder_Start(HAL_TIM_Instance_t instance);

uint16_t HAL_TIM_Encoder_GetCount(HAL_TIM_Instance_t instance);

void HAL_TIM_Encoder_Reset(HAL_TIM_Instance_t instance);

void TIM2_IRQHandler(void);

HAL_Status_t HAL_TIM_PWM_Init(HAL_TIM_Instance_t instance,
                               uint32_t freq_hz, uint16_t max_duty);

HAL_Status_t HAL_TIM_PWM_Start(HAL_TIM_Instance_t instance);

void HAL_TIM_PWM_SetDuty(HAL_TIM_Instance_t instance, uint16_t duty);

void HAL_TIM_PWM_Stop(HAL_TIM_Instance_t instance);

HAL_Status_t HAL_TIM_Base_Init(HAL_TIM_Instance_t instance, uint16_t period_ms);

HAL_Status_t HAL_TIM_Base_Start_IT(HAL_TIM_Instance_t instance);

void TIM4_IRQHandler(void);

HAL_Status_t HAL_WDG_Init(uint32_t timeout_ms);

void HAL_WDG_Refresh(void);

#endif
