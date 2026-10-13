#include "pid_controller.h"

void PID_Init(WF_PIDState_t *pid, int32_t kp, int32_t ki, int32_t kd) {
    if (pid == (WF_PIDState_t*)0) {
        return;
    }

    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;

    pid->setpoint         = 0;
    pid->integral         = 0;
    pid->prev_error       = 0;
    pid->output           = 0;
    pid->enabled          = 1;
    pid->stall_detected   = 0;
    pid->stall_counter_ms = 0;
    pid->last_position    = 0;
    pid->current_position = 0;
    pid->error            = 0;
}

HAL_Status_t PID_SetSetpoint(WF_PIDState_t *pid, int32_t target_pos) {
    if (pid == (WF_PIDState_t*)0) {
        return HAL_ERROR;
    }

    HAL_Status_t status = HAL_OK;
    if (target_pos < WF_MOTOR_POS_MIN) {
        target_pos = WF_MOTOR_POS_MIN;
        status = HAL_ERROR;
    } else if (target_pos > WF_MOTOR_POS_MAX) {
        target_pos = WF_MOTOR_POS_MAX;
        status = HAL_ERROR;
    }

    pid->setpoint = target_pos;
    return status;
}

int32_t PID_Update(WF_PIDState_t *pid, int32_t measured_pos) {
    if (pid == (WF_PIDState_t*)0) {
        return 0;
    }

    if (!pid->enabled) {
        pid->output = 0;
        return 0;
    }

    pid->current_position = measured_pos;
    int32_t error = pid->setpoint - measured_pos;
    pid->error = error;

    int64_t p_term = ((int64_t)pid->Kp * error) / WF_PID_GAIN_SCALE;

    pid->integral += error;
    if (pid->integral > WF_PID_INTEGRAL_MAX) {
        pid->integral = WF_PID_INTEGRAL_MAX;
    } else if (pid->integral < WF_PID_INTEGRAL_MIN) {
        pid->integral = WF_PID_INTEGRAL_MIN;
    }
    int64_t i_term = ((int64_t)pid->Ki * pid->integral) / WF_PID_GAIN_SCALE;

    int32_t derivative = error - pid->prev_error;
    int64_t d_term = ((int64_t)pid->Kd * derivative) / WF_PID_GAIN_SCALE;
    pid->prev_error = error;

    int64_t total_out = p_term + i_term + d_term;

    int32_t pos_delta = measured_pos - pid->last_position;
    if (pos_delta < 0) pos_delta = -pos_delta;

    if (total_out > (WF_PID_OUTPUT_MAX / 2) || total_out < (WF_PID_OUTPUT_MIN / 2)) {
        if (pos_delta < WF_STALL_THRESHOLD) {
            pid->stall_counter_ms += WF_PID_PERIOD_MS;
            if (pid->stall_counter_ms >= WF_STALL_TIMEOUT_MS) {
                pid->stall_detected = 1;

                total_out = (total_out * WF_STALL_PWM_REDUCE) / 100;
            }
        } else {
            pid->stall_counter_ms = 0;
            pid->stall_detected = 0;
        }
    } else {
        pid->stall_counter_ms = 0;
        pid->stall_detected = 0;
    }
    pid->last_position = measured_pos;

    if (total_out > WF_PID_OUTPUT_MAX) {
        total_out = WF_PID_OUTPUT_MAX;
    } else if (total_out < WF_PID_OUTPUT_MIN) {
        total_out = WF_PID_OUTPUT_MIN;
    }

    pid->output = (int32_t)total_out;
    return pid->output;
}

void PID_Reset(WF_PIDState_t *pid) {
    if (pid == (WF_PIDState_t*)0) {
        return;
    }
    pid->integral         = 0;
    pid->prev_error       = 0;
    pid->output           = 0;
    pid->stall_detected   = 0;
    pid->stall_counter_ms = 0;
}

void PID_Enable(WF_PIDState_t *pid, uint8_t enable) {
    if (pid == (WF_PIDState_t*)0) {
        return;
    }
    pid->enabled = enable ? 1 : 0;
    if (!pid->enabled) {
        pid->output = 0;
    }
}

uint8_t PID_IsStallDetected(const WF_PIDState_t *pid) {
    if (pid == (WF_PIDState_t*)0) {
        return 0;
    }
    return pid->stall_detected;
}
