/*
 * @Author       : DragonYH 1016633827@qq.com
 * @Date         : 2024-07-29 13:32:07
 * @LastEditors  : DragonYH 1016633827@qq.com
 * @LastEditTime : 2024-08-01 06:48:05
 * @FilePath     : \CP_ACAC_Parallel_H743IIT6_Hollies_Provincial\User\Src\user_exit.c
 * @Description  : 用户中断实现
 *
 * Copyright (c) 2024 by DragonYH, All Rights Reserved.
 */

#include "user_task.h"
#include "user_exit.h"
#include "pll.h"
#include "ad7606.h"
#include "spi.h"
#include "main.h"
#include "tim.h"
#include "dac.h"
#include "arm_math.h"
#include <malloc.h>

static void getVoltageCurrent(void);
static void calcEffectiveValue(void);
static void normalize(void);

allSignal_t *allSignal;

/**
 * @brief    外部中断
 * @param    GPIO_Pin
 * @return
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == AD7606_BUSY_Pin)
    {
        getVoltageCurrent();  /* 获取电压电流 */
        calcEffectiveValue(); /* 计算有效值 */
        normalize();          /* 标幺化 */
    }
}

/**
 * @brief    获取电压电流
 * @return
 */
static void getVoltageCurrent(void)
{
    static float adcValue[8];
    ad7606_GetValue(&hspi2, 5, adcValue);

    allSignal->voltageOut = adcValue[2] * 28.209f;      /* 输出电压 */
    allSignal->currentOut = adcValue[4] * 1.0124403f;   /* 输出电流 */
    allSignal->currentOutAll = adcValue[3] * 2.5263158; /* 输出总电流 */
}

/**
 * @brief    计算输出电压电流有效值
 */
static void calcEffectiveValue(void)
{
    static volatile uint16_t cnt = 0;

    static float VoRms[400] = {0};
    static float IoRms[400] = {0};
    static float IoRmsAll[400] = {0};

    VoRms[cnt] = allSignal->voltageOut;
    IoRms[cnt] = allSignal->currentOut;
    IoRmsAll[cnt] = allSignal->currentOutAll;

    if (++cnt == 400)
    {
        cnt = 0;
        arm_rms_f32(VoRms, 400, &allSignal->voltageOutRms);
        arm_rms_f32(IoRms, 400, &allSignal->currentOutRms);
        arm_rms_f32(IoRmsAll, 400, &allSignal->currentOutAllRms);

        allSignal->voltageOutRms -= 0.05f;
        allSignal->voltageOutRms -= 0.2085223f * (allSignal->currentOutRms - 1.5f);
    }
}

/**
 * @brief    标幺化
 */
static void normalize(void)
{
    allSignal->pllVoltage->basic->input[0] /= UBASE;
}

/**
 * @brief    信号初始化
 * @param    **allSignal 信号指针的指针
 * @return
 */
void allSignal_Init(allSignal_t **allSignal)
{
    (*allSignal) = (allSignal_t *)malloc(sizeof(allSignal_t));
    (*allSignal)->pllVoltage = (pll_Signal_t *)malloc(sizeof(pll_Signal_t));
    (*allSignal)->pid = (PID_t *)malloc(sizeof(PID_t));

    (*allSignal)->pid->out = 0.9f;

    pll_Init(&((*allSignal)->pllVoltage), 50, 20000);
}
