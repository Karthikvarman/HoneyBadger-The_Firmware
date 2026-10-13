#ifndef PID_CONFIG_H
#define PID_CONFIG_H

#include <stdint.h>

#define WF_PID_KP               500
#define WF_PID_KI               50
#define WF_PID_KD               100
#define WF_PID_GAIN_SCALE       1000

#define WF_PID_INTEGRAL_MAX     500000L
#define WF_PID_INTEGRAL_MIN     (-500000L)

#define WF_PID_OUTPUT_MAX       999
#define WF_PID_OUTPUT_MIN       (-999)

#define WF_MOTOR_POS_MIN        (-1000000L)
#define WF_MOTOR_POS_MAX        1000000L

#define WF_MOTOR_SETPOINT_INTERVAL_MS   5000U
#define WF_MOTOR_SETPOINT_TIMEOUT_MS    10000U

#define WF_STALL_THRESHOLD      5
#define WF_STALL_TIMEOUT_MS     1000U
#define WF_STALL_PWM_REDUCE     50U

typedef struct {

    int32_t     Kp;
    int32_t     Ki;
    int32_t     Kd;

    int32_t     setpoint;
    int32_t     integral;
    int32_t     prev_error;
    int32_t     output;

    uint8_t     enabled;
    uint8_t     stall_detected;
    uint16_t    stall_counter_ms;

    int32_t     last_position;
    int32_t     current_position;
    int32_t     error;
} WF_PIDState_t;

#endif
