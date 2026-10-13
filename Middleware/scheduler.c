#include "scheduler.h"
#include "../HAL/hal_timer.h"

static WF_Task_t task_table[WF_MAX_TASKS];
static uint8_t   task_count = 0;

static void sort_tasks_by_priority(void) {
    for (uint8_t i = 1; i < task_count; i++) {
        WF_Task_t key = task_table[i];
        int32_t j = (int32_t)i - 1;

        while (j >= 0 && task_table[j].priority > key.priority) {
            task_table[j + 1] = task_table[j];
            j--;
        }
        task_table[j + 1] = key;
    }
}

void Scheduler_Init(void) {
    task_count = 0;
    for (uint8_t i = 0; i < WF_MAX_TASKS; i++) {
        task_table[i].name          = (const char*)0;
        task_table[i].period_ms     = 0;
        task_table[i].priority      = 255;
        task_table[i].execute       = (WF_TaskFunc_t)0;
        task_table[i].tick_count    = 0;
        task_table[i].ready         = 0;
        task_table[i].last_exec_ms  = 0;
        task_table[i].exec_count    = 0;
        task_table[i].worst_exec_us = 0;
    }
}

HAL_Status_t Scheduler_AddTask(const char *name, uint16_t period_ms,
                                uint8_t priority, WF_TaskFunc_t func) {
    if (func == (WF_TaskFunc_t)0 || task_count >= WF_MAX_TASKS) {
        return HAL_ERROR;
    }

    WF_Task_t *task = &task_table[task_count];
    task->name          = name;
    task->period_ms     = period_ms;
    task->priority      = priority;
    task->execute       = func;
    task->tick_count    = 0;
    task->ready         = 0;
    task->last_exec_ms  = 0;
    task->exec_count    = 0;
    task->worst_exec_us = 0;

    task_count++;
    sort_tasks_by_priority();

    return HAL_OK;
}

void Scheduler_TickHook(void) {

    for (uint8_t i = 0; i < task_count; i++) {
        if (task_table[i].period_ms == 0) {
            continue;
        }

        task_table[i].tick_count++;
        if (task_table[i].tick_count >= task_table[i].period_ms) {
            task_table[i].ready = 1;
            task_table[i].tick_count = 0;
        }
    }
}

void Scheduler_Dispatch(void) {

    for (uint8_t i = 0; i < task_count; i++) {
        if (task_table[i].ready && task_table[i].execute != (WF_TaskFunc_t)0) {

            task_table[i].ready = 0;

            uint32_t start_ms = HAL_GetTick();

            task_table[i].execute();

            task_table[i].last_exec_ms = HAL_GetTick();
            task_table[i].exec_count++;

            uint32_t duration_ms = task_table[i].last_exec_ms - start_ms;
            uint16_t duration_us = (uint16_t)(duration_ms * 1000U);
            if (duration_us > task_table[i].worst_exec_us) {
                task_table[i].worst_exec_us = duration_us;
            }
        }
    }
}

uint8_t Scheduler_GetTaskCount(void) {
    return task_count;
}

const WF_Task_t* Scheduler_GetTaskTable(void) {
    return task_table;
}
