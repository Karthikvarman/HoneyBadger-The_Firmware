#include "motor_control.h"
#include "../Config/wf_config.h"
#include "../Config/pid_config.h"
#include "../HAL/hal_timer.h"
#include "../HAL/hal_gpio.h"
#include "../Middleware/pid_controller.h"

static WF_PIDState_t g_pid_state;

extern volatile int32_t encoder_overflow_count;
extern volatile uint8_t pid_ready_flag;

void MotorControl_Init(void) {
    PID_Init(&g_pid_state, WF_PID_KP, WF_PID_KI, WF_PID_KD);

    HAL_GPIO_Write(WF_MOTOR_DIR_A_PORT, WF_MOTOR_DIR_A_PIN, HAL_PIN_LOW);
    HAL_GPIO_Write(WF_MOTOR_DIR_B_PORT, WF_MOTOR_DIR_B_PIN, HAL_PIN_LOW);

    HAL_TIM_Encoder_Reset(HAL_TIM_2);
    HAL_TIM_Encoder_Start(HAL_TIM_2);

    HAL_TIM_PWM_SetDuty(HAL_TIM_3, 0);
    HAL_TIM_PWM_Start(HAL_TIM_3);
}

int32_t MotorControl_GetCurrentPosition(void) {
    uint16_t raw_cnt = HAL_TIM_Encoder_GetCount(HAL_TIM_2);
    int32_t total_pos = ((int32_t)encoder_overflow_count * 65536L) + (int32_t)raw_cnt;
    return total_pos;
}

void MotorControl_ExecutePID(void) {

    int32_t current_pos = MotorControl_GetCurrentPosition();

    int32_t output = PID_Update(&g_pid_state, current_pos);

    if (output >= 0) {
        HAL_GPIO_Write(WF_MOTOR_DIR_A_PORT, WF_MOTOR_DIR_A_PIN, HAL_PIN_HIGH);
        HAL_GPIO_Write(WF_MOTOR_DIR_B_PORT, WF_MOTOR_DIR_B_PIN, HAL_PIN_LOW);
        HAL_TIM_PWM_SetDuty(HAL_TIM_3, (uint16_t)output);
    } else {
        HAL_GPIO_Write(WF_MOTOR_DIR_A_PORT, WF_MOTOR_DIR_A_PIN, HAL_PIN_LOW);
        HAL_GPIO_Write(WF_MOTOR_DIR_B_PORT, WF_MOTOR_DIR_B_PIN, HAL_PIN_HIGH);
        HAL_TIM_PWM_SetDuty(HAL_TIM_3, (uint16_t)(-output));
    }
}

HAL_Status_t MotorControl_SetTargetPosition(int32_t setpoint) {
    return PID_SetSetpoint(&g_pid_state, setpoint);
}

int32_t MotorControl_GetTargetPosition(void) {
    return g_pid_state.setpoint;
}

const WF_PIDState_t* MotorControl_GetPIDState(void) {
    return &g_pid_state;
}
