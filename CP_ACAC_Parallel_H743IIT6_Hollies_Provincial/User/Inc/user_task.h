#ifndef USER_TASK_H
#define USER_TASK_H

#include "main.h"

/* 运行状态 */
enum state
{
    START = 0,
    RUN,
    FAULT
};

void UserInit();

void StartStateLED(void *argument);
void StartOledDisplay(void *argument);
void StartKeyControl(void *argument);
void StartMainControl(void *argument);
void StartStateControl(void *argument);

#endif
