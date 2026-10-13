#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include <stdint.h>
#include "../Config/wf_config.h"
#include "../Config/pid_config.h"
#include "../HAL/hal_common.h"

void PID_Init(WF_PIDState_t *pid, int32_t kp, int32_t ki, int32_t kd);

HAL_Status_t PID_SetSetpoint(WF_PIDState_t *pid, int32_t target_pos);

int32_t PID_Update(WF_PIDState_t *pid, int32_t measured_pos);

void PID_Reset(WF_PIDState_t *pid);

void PID_Enable(WF_PIDState_t *pid, uint8_t enable);

uint8_t PID_IsStallDetected(const WF_PIDState_t *pid);

#endif
