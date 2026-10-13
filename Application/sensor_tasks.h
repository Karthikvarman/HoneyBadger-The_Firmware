#ifndef SENSOR_TASKS_H
#define SENSOR_TASKS_H

#include "../Config/sensor_config.h"

void SensorTasks_Init(void);

void Task_Read_S1(void);

void Task_Read_S2(void);

void Task_Read_S3(void);

void Task_Read_S4(void);

void Task_Read_S5(void);

void Task_Read_S6(void);

void Task_Read_I2C_All(void);

#endif
