/*
 * @Author       : DragonYH 1016633827@qq.com
 * @Date         : 2024-07-29 13:32:41
 * @LastEditors  : DragonYH 1016633827@qq.com
 * @LastEditTime : 2024-07-29 18:32:21
 * @FilePath     : \CP_ACAC_Parallel_H743IIT6_Hollies_Provincial\User\Src\pll.c
 * @Description  : 锁相环实现
 *
 * Copyright (c) 2024 by DragonYH, All Rights Reserved.
 */

#include "pll.h"
#include "arm_math.h"
#include <stdlib.h>

/**
 * @brief Sogi变换
 * @param sogi sogi指针
 * @param input 输入信号
 */
static void pll_Sogi(pll_Sogi_t *sogi, float *input)
{
    sogi->alpha[0] = sogi->b0 * input[0] - sogi->b0 * input[2] + sogi->a1 * sogi->alpha[1] + sogi->a2 * sogi->alpha[2];
    sogi->beta[0] = sogi->lamda * sogi->b0 * (input[0] + 2 * input[1] + input[2]) + sogi->a1 * sogi->beta[1] + sogi->a2 * sogi->beta[2];

    input[2] = input[1];
    input[1] = input[0];
    sogi->alpha[2] = sogi->alpha[1];
    sogi->alpha[1] = sogi->alpha[0];
    sogi->beta[2] = sogi->beta[1];
    sogi->beta[1] = sogi->beta[0];
}

/**
 * @brief 电压信号参数初始化
 * @param signal 信号指针
 * @param f 信号频率(典型值:50)
 * @param F 采样频率(典型值:20000)
 */
void pll_Init(pll_Signal_t **signal, float f, uint16_t F)
{
    /* 分配内存空间 */
    (*signal) = (pll_Signal_t *)malloc(sizeof(pll_Signal_t));
    (*signal)->basic = (pll_Basic_t *)malloc(sizeof(pll_Basic_t));
    (*signal)->basic->sogi = (pll_Sogi_t *)malloc(sizeof(pll_Sogi_t));
    (*signal)->pid = (PID_t *)malloc(sizeof(PID_t));

    /* 输入初始化赋值 */
    (*signal)->basic->input[0] = 0.f;
    (*signal)->basic->input[1] = 0.f;
    (*signal)->basic->input[2] = 0.f;

    /* 有效值初始化赋值 */
    (*signal)->basic->rms = 0.f;

    /* sogi变换相关变量初始化赋值 */
    (*signal)->basic->sogi->alpha[0] = 0.f;
    (*signal)->basic->sogi->alpha[1] = 0.f;
    (*signal)->basic->sogi->alpha[2] = 0.f;
    (*signal)->basic->sogi->beta[0] = 0.f;
    (*signal)->basic->sogi->beta[1] = 0.f;
    (*signal)->basic->sogi->beta[2] = 0.f;

    /* park变换相关变量初始化赋值 */
    (*signal)->basic->park_d = 0.f;
    (*signal)->basic->park_q = 0.f;

    (*signal)->basic->omiga0 = 2 * PI * f; /* f典型值50 */
    (*signal)->basic->Ts = 1.f / F;        /* F典型值20000 */

    (*signal)->theta = 0.f;

    /* 计算sogi中间量 */
    (*signal)->basic->sogi->k = 1.414f;
    (*signal)->basic->sogi->lamda = 0.5f * (*signal)->basic->omiga0 * (*signal)->basic->Ts;
    (*signal)->basic->sogi->x = 2.f * (*signal)->basic->sogi->k * (*signal)->basic->omiga0 * (*signal)->basic->Ts;
    (*signal)->basic->sogi->y = (*signal)->basic->omiga0 * (*signal)->basic->Ts * (*signal)->basic->omiga0 * (*signal)->basic->Ts;
    (*signal)->basic->sogi->b0 = (*signal)->basic->sogi->x / ((*signal)->basic->sogi->x + (*signal)->basic->sogi->y + 4);
    (*signal)->basic->sogi->a1 = (8 - 2.f * (*signal)->basic->sogi->y) / ((*signal)->basic->sogi->x + (*signal)->basic->sogi->y + 4);
    (*signal)->basic->sogi->a2 = ((*signal)->basic->sogi->x - (*signal)->basic->sogi->y - 4) / ((*signal)->basic->sogi->x + (*signal)->basic->sogi->y + 4);

    /* 初始化pid参数 */
    float ki = (*signal)->basic->omiga0 * (*signal)->basic->omiga0;
    float kp = sqrtf(2) * sqrtf(ki);
    pid_Init((*signal)->pid, kp, ki, 0, 50 * PI, -20 * PI);
}

/**
 * @brief 电压锁相控制
 * @param signal 电压信号指针
 */
void pll_Control(pll_Signal_t *signal)
{
    /* 对信号先进行sogi变换，得到两个相位相差90度的信号 */
    pll_Sogi(signal->basic->sogi, signal->basic->input);

    /* 再对信号sogi变换后的信号进行park变换 */
    float sinTheta = arm_sin_f32(signal->theta);
    float cosTheta = arm_cos_f32(signal->theta);
    arm_park_f32(signal->basic->sogi->alpha[0], signal->basic->sogi->beta[0], &signal->basic->park_d, &signal->basic->park_q, sinTheta, cosTheta);

    /* 将park变换后的q送入PI控制器  输入值为设定值和采样值的误差 */
    pid(signal->pid, signal->basic->park_q, 0); /* pid的输出值为旋转坐标系角速度 */

    /* 更新theta */
    signal->theta += (signal->pid->out + signal->basic->omiga0) * signal->basic->Ts;
    signal->theta = (float)fmod(signal->theta, 2 * PI);
}
