#ifndef USER_TASK_H
#define USER_TASK_H

void StartDefaultTask(void *argument);
void StartChipTemperatureTask(void *argument);
void StartOledDisplayTask(void *argument);
void StartKeyTask(void *argument);
void StartDCSampingTask(void *argument);

#endif
