#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include "../HAL/hal_common.h"

typedef void (*WF_TaskFunc_t)(void);

typedef struct {
    const char*         name;
    uint16_t            period_ms;
    uint8_t             priority;
    WF_TaskFunc_t       execute;

    volatile uint16_t   tick_count;
    volatile uint8_t    ready;
    uint32_t            last_exec_ms;
    uint32_t            exec_count;
    uint16_t            worst_exec_us;
} WF_Task_t;

#define WF_MAX_TASKS    16

void Scheduler_Init(void);

HAL_Status_t Scheduler_AddTask(const char *name, uint16_t period_ms,
                                uint8_t priority, WF_TaskFunc_t func);

void Scheduler_Dispatch(void);

void Scheduler_TickHook(void);

uint8_t Scheduler_GetTaskCount(void);

const WF_Task_t* Scheduler_GetTaskTable(void);

#endif
