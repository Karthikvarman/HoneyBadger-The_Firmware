#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <stdint.h>
#include "../Config/pid_config.h"
#include "../HAL/hal_common.h"

void MotorControl_Init(void);

void MotorControl_ExecutePID(void);

HAL_Status_t MotorControl_SetTargetPosition(int32_t setpoint);

int32_t MotorControl_GetTargetPosition(void);

int32_t MotorControl_GetCurrentPosition(void);

const WF_PIDState_t* MotorControl_GetPIDState(void);

#endif
