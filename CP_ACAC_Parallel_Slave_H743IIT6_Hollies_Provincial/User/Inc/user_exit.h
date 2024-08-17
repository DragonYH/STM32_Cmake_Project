#ifndef USER_EXIT_H
#define USER_EXIT_H

#include "pll.h"

#define UBASE_RMS (40.f)
#define UBASE (UBASE_RMS * 1.414f) /* 电压基值 */
#define TIM_PERIOD (11999)

typedef struct allSignal_t
{
    pll_Signal_t *pllVoltage; /* 输入电压锁相 */
    float voltageOut;         /* 输出电压 */
    float currentOut;         /* 输出电流 */
    float voltageOutRms;      /* 输出电压有效值 */
    float currentOutRms;      /* 输出电流有效值 */
    PID_t *pid;               /* PID控制 */
} allSignal_t;

extern allSignal_t *allSignal;

void allSignal_Init(allSignal_t **allSignal);

#endif
