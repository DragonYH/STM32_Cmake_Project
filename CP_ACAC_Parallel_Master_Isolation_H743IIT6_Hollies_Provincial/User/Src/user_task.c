/*
 * @Author       : DragonYH 1016633827@qq.com
 * @Date         : 2024-07-29 13:32:33
 * @LastEditors  : DragonYH 1016633827@qq.com
 * @LastEditTime : 2024-08-01 10:08:12
 * @FilePath     : \CP_ACAC_Parallel_Master_Isolation_H743IIT6_Hollies_Provincial\User\Src\user_task.c
 * @Description  : FreeRTOS任务实现
 *
 * Copyright (c) 2024 by DragonYH, All Rights Reserved.
 */

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "user_exit.h"
#include "user_task.h"
#include "oled.h"
#include "ad7606.h"
#include "spi.h"
#include "tim.h"
#include "dac.h"
#include "pid.h"
#include <stdio.h>

enum state deviceState = START; /* 设备状态 */

static float UoValueSet = 30.f; /* 输出电压设定值 */
static float IoRatioSet = 1.f;  /* 输出电电流比例设定值 */

enum MODE
{
    VOLTAGE_MODE = 0,
    CURRENT_MODE
};
enum MODE mode = VOLTAGE_MODE;

void UserInit()
{
    allSignal_Init(&allSignal);
    ad7606_Init();
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim8, TIM_CHANNEL_1);
}

/**
 * @brief    状态指示灯任务
 * @param    *argument
 * @return
 */
void StartStateLED(void *argument)
{
    /* USER CODE BEGIN StartStateLED */
    UNUSED(argument);
    /* Infinite loop */
    for (;;)
    {
        switch (deviceState)
        {
        case START: /* 启动状态 */
            HAL_GPIO_WritePin(GPIOI, GPIO_PIN_0, GPIO_PIN_RESET);
            break;
        case RUN: /* 正常运行 */
            HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_0);
            osDelay(100);
            HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_0);
            osDelay(200);
            HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_0);
            osDelay(100);
            HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_0);
            osDelay(200);
            HAL_GPIO_WritePin(GPIOI, GPIO_PIN_0, GPIO_PIN_SET);
            osDelay(1000);
            break;
        case FAULT: /* 保护状态 */
            HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_0);
            osDelay(50);
            break;
        default:
            break;
        }
    }
    /* USER CODE END StartStateLED */
}

/**
 * @brief    OLED显示任务
 * @param    *argument
 * @return
 */
void StartOledDisplay(void *argument)
{
    /* USER CODE BEGIN StartoledDisplay */
    UNUSED(argument);
    OLED_Init();
    static char str[22];
    /* Infinite loop */
    for (;;)
    {
        snprintf(str, sizeof(str), "Uo:%5.2f Io:%4.2f", allSignal->voltageOutRms, allSignal->currentOutRms);
        OLED_ShowString(0, 0, (uint8_t *)str, 12);

        snprintf(str, sizeof(str), "USet:%4.1f ISet:%3.1f", UoValueSet, IoRatioSet);
        OLED_ShowString(0, 12, (uint8_t *)str, 12);

        float Iratio = allSignal->currentOutRms / (allSignal->currentOutAllRms - allSignal->currentOutRms);
        if (Iratio < 0)
        {
            Iratio = 0;
        }
        snprintf(str, sizeof(str), "IAll:%4.2f IRat:%4.2f", allSignal->currentOutAllRms, Iratio);
        OLED_ShowString(0, 24, (uint8_t *)str, 12);

        snprintf(str, sizeof(str), "Duty:%4.2f", allSignal->pid->out);
        OLED_ShowString(0, 36, (uint8_t *)str, 12);

        snprintf(str, sizeof(str), "Mode:%s", mode == VOLTAGE_MODE ? "VMODE" : "CMODE");
        OLED_ShowString(34, 48, (uint8_t *)str, 12);

        OLED_Refresh();
        osDelay(100);
    }
    /* USER CODE END StartoledDisplay */
}

/*******************************************************************************************/
/*************************************** 按键相关函数 ***************************************/
uint8_t GetKeyNum()
{
    uint8_t keyNum = 5;
    if (HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == GPIO_PIN_RESET)
    {
        keyNum = 0;
    }
    else if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
    {
        keyNum = 1;
    }
    else if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET)
    {
        keyNum = 2;
    }
    else if (HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET)
    {
        keyNum = 3;
    }
    else
    {
        keyNum = 5;
    }
    return keyNum;
}

/**
 * @brief    按键控制任务
 * @param    *argument
 * @return
 */
void StartKeyControl(void *argument)
{
    UNUSED(argument);
    for (;;)
    {
        uint8_t keyNum;
        keyNum = GetKeyNum();

        switch (keyNum)
        {
        case 0:
            osDelay(100);
            keyNum = GetKeyNum();
            if (keyNum == 0)
            {
                if (mode != CURRENT_MODE)
                    mode++;
                else
                    mode = VOLTAGE_MODE;
            }
            break;
        case 1:
            osDelay(100);
            keyNum = GetKeyNum();
            if (keyNum == 1)
            {
                if (mode == VOLTAGE_MODE)
                {
                    if (UoValueSet >= 32)
                    {
                        UoValueSet = 32.f;
                    }
                    else
                    {
                        UoValueSet += 0.5f;
                    }
                }

                if (mode == CURRENT_MODE)
                {
                    if (IoRatioSet >= 2.f)
                    {
                        IoRatioSet = 2.f;
                    }
                    else
                    {
                        IoRatioSet += 0.1f;
                    }
                }
            }
            break;
        case 2:
            osDelay(100);
            keyNum = GetKeyNum();
            if (keyNum == 2)
            {
                if (mode == VOLTAGE_MODE)
                {
                    if (UoValueSet <= 1)
                    {
                        UoValueSet = 1.f;
                    }
                    else
                    {
                        UoValueSet -= 0.5f;
                    }
                }

                if (mode == CURRENT_MODE)
                {
                    if (IoRatioSet <= 0.5f)
                    {
                        IoRatioSet = 0.5f;
                    }
                    else
                    {
                        IoRatioSet -= 0.1f;
                    }
                }
            }
            break;
        default:
            break;
        }
        osDelay(10);
    }
}

/**
 * @brief    pwm控制
 * @return
 */
void pwmControl(void)
{
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, allSignal->pid->out * TIM_PERIOD);
}

/**
 * @brief    主控制程序
 * @param    *argument
 * @return
 */
void StartMainControl(void *argument)
{
    /* USER CODE BEGIN StartVoltageControl */
    UNUSED(argument);
    pid_Init(allSignal->pid, 0.4f, 0.2f, 0.f, 0.98f, 0.02f);
    allSignal->pid->out = 0.91f;
    ad7606_Start(&htim2, TIM_CHANNEL_1);
    /* Infinite loop */
    for (;;)
    {
        if (deviceState == RUN)
        {
            if (mode == VOLTAGE_MODE)
            {
                allSignal->pid->kp = 0.4f;
                allSignal->pid->ki = 0.2f;
                // allSignal->pid->Max = 0.98f;
                // allSignal->pid->Min = 0.02f;
                allSignal->pid->Max = 0.92f;
                allSignal->pid->Min = 0.87f;

                // pid(allSignal->pid, UoValueSet / UBASE_RMS, (allSignal->voltageOutRms) / UBASE_RMS);
            }
            else if (mode == CURRENT_MODE)
            {

                allSignal->pid->kp = 0.04f;
                allSignal->pid->ki = 0.02f;
                allSignal->pid->Max = 0.92f;
                allSignal->pid->Min = 0.87f;

                float Iratio = allSignal->currentOutRms / (allSignal->currentOutAllRms - allSignal->currentOutRms);
                if (Iratio < 0)
                {
                    Iratio = 0;
                }
                else if (Iratio > 3)
                {
                    Iratio = 3;
                }
                // pid(allSignal->pid, IoRatioSet, Iratio);
            }
            pwmControl();
        }
        osDelay(50);
    }
    /* USER CODE END StartVoltageControl */
}

/**
 * @brief    状态控制任务
 * @param    *argument
 * @return
 */
void StartStateControl(void *argument)
{
    UNUSED(argument);
    for (;;)
    {
        if (deviceState == START)
        {
            // osDelay(500);
            if (allSignal->voltageOutRms > 10.f)
            {
                mode = CURRENT_MODE;
                deviceState = RUN;
                allSignal->pid->out = 0.91f;
                HAL_GPIO_WritePin(IR2104_SD1_GPIO_Port, IR2104_SD1_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(IR2104_SD2_GPIO_Port, IR2104_SD2_Pin, GPIO_PIN_SET);
                vTaskDelete(NULL);
            }
            else
            {
                mode = VOLTAGE_MODE;
                deviceState = RUN;
                allSignal->pid->out = 0.91f;
                HAL_GPIO_WritePin(IR2104_SD1_GPIO_Port, IR2104_SD1_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(IR2104_SD2_GPIO_Port, IR2104_SD2_Pin, GPIO_PIN_SET);
                vTaskDelete(NULL);
            }
        }
        osDelay(100);
    }
}
