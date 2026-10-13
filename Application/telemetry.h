#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "../HAL/hal_common.h"

void Telemetry_Init(void);

void Task_Telemetry_TX(void);

void Telemetry_ProcessRX(void);

uint32_t Telemetry_GetLastCmdTimestamp(void);

#endif
