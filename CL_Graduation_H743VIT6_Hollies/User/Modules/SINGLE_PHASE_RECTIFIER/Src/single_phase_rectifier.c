/*
 * @Author       : DragonYH 1016633827@qq.com
 * @Date         : 2024-07-26 10:50:02
 * @LastEditors  : DragonYH 1016633827@qq.com
 * @LastEditTime : 2025-05-19 05:20:45
 * @FilePath     : \CL_Graduation_H743VIT6_Hollies\User\Modules\SINGLE_PHASE_RECTIFIER\Src\single_phase_rectifier.c
 * @Description  :
 *
 * Copyright (c) 2024 by DragonYH, All Rights Reserved.
 */

#include "single_phase_rectifier.h"
#include "pid.h"
#include <math.h>
#include <stdlib.h>
#include <malloc.h>
#include "arm_math.h"

static void pll_Sogi(Sogi *sogi, float *input);

/**
 * @brief 电压信号参数初始化
 * @param signal 信号指针
 * @param f 信号频率(典型值:50)
 * @param F 采样频率(典型值:20000)
 */
void single_Phase_Init_V(single_Phase_Signal_V **signal, float f, uint16_t F)
{
    /* 分配内存空间 */
    (*signal) = (single_Phase_Signal_V *)malloc(sizeof(single_Phase_Signal_V));
    (*signal)->basic = (single_Phase_Signal_Basic *)malloc(sizeof(single_Phase_Signal_Basic));
    (*signal)->basic->sogi = (Sogi *)malloc(sizeof(Sogi));
    (*signal)->pid = (PID *)malloc(sizeof(PID));

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
    float kp = sqrt(2) * sqrt(ki);
    pid_Init((*signal)->pid, kp, ki, 0, 50 * PI, -20 * PI);
}

/**
 * @brief 电流信号参数初始化
 * @param signal 信号指针
 * @param f 信号频率(典型值:50)
 * @param F 采样频率(典型值:20000)
 */
void single_Phase_Init_I(single_Phase_Signal_I **signal, float f, uint16_t F)
{
    /* 分配内存空间 */
    (*signal) = (single_Phase_Signal_I *)malloc(sizeof(single_Phase_Signal_I));
    (*signal)->basic = (single_Phase_Signal_Basic *)malloc(sizeof(single_Phase_Signal_Basic));
    (*signal)->basic->sogi = (Sogi *)malloc(sizeof(Sogi));
    (*signal)->pid_d = (PID *)malloc(sizeof(PID));
    (*signal)->pid_q = (PID *)malloc(sizeof(PID));

    /* 初始化赋值 */
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

    (*signal)->basic->omiga0 = 2.f * PI * f; /* f典型值50 */
    (*signal)->basic->Ts = 1.f / F;          /* F典型值20000 */

    /* park逆变换相关变量初始化赋值 */
    (*signal)->park_inv_alpha = 0.f;
    (*signal)->park_inv_beta = 0.f;

    (*signal)->CorL = 0;    /* 0:感性 1:容性 */
    (*signal)->L = 0.0043f; /* 1mH */

    /* 计算sogi中间量 */
    (*signal)->basic->sogi->k = 1.414f;
    (*signal)->basic->sogi->lamda = 0.5f * (*signal)->basic->omiga0 * (*signal)->basic->Ts;
    (*signal)->basic->sogi->x = 2.f * (*signal)->basic->sogi->k * (*signal)->basic->omiga0 * (*signal)->basic->Ts;
    (*signal)->basic->sogi->y = (*signal)->basic->omiga0 * (*signal)->basic->Ts * (*signal)->basic->omiga0 * (*signal)->basic->Ts;
    (*signal)->basic->sogi->b0 = (*signal)->basic->sogi->x / ((*signal)->basic->sogi->x + (*signal)->basic->sogi->y + 4);
    (*signal)->basic->sogi->a1 = (8 - 2.f * (*signal)->basic->sogi->y) / ((*signal)->basic->sogi->x + (*signal)->basic->sogi->y + 4);
    (*signal)->basic->sogi->a2 = ((*signal)->basic->sogi->x - (*signal)->basic->sogi->y - 4) / ((*signal)->basic->sogi->x + (*signal)->basic->sogi->y + 4);

    /* 在调整取值范围时看实际输出值逐渐逼近，防止上电瞬间电流过大 */
    float tmp = 3.f * (*signal)->basic->Ts / sqrtf(3);
    float kp = (*signal)->L / tmp;
    float ki = 0.15f / tmp;
    pid_Init((*signal)->pid_d, kp, ki, 0, 10000.f, -10000.f);
    pid_Init((*signal)->pid_q, kp, ki, 0, 10000.f, -10000.f);
}

/**
 * @brief 电压锁相控制
 * @param signal_V 电压信号指针
 */
void single_Phase_PLL_V(single_Phase_Signal_V *signal_V)
{
    /* 对信号先进行sogi变换，得到两个相位相差90度的信号 */
    pll_Sogi(signal_V->basic->sogi, signal_V->basic->input);

    /* 再对信号sogi变换后的信号进行park变换 */
    float sinTheta = arm_sin_f32(signal_V->theta);
    float cosTheta = arm_cos_f32(signal_V->theta);
    arm_park_f32(signal_V->basic->sogi->alpha[0], signal_V->basic->sogi->beta[0], &signal_V->basic->park_d, &signal_V->basic->park_q, sinTheta, cosTheta);

    /* 将park变换后的q送入PI控制器  输入值为设定值和采样值的误差 */
    pid_positional(signal_V->pid, signal_V->basic->park_q, 0); /* pid的输出值为旋转坐标系角速度 */

    /* 更新theta */
    signal_V->theta += (signal_V->pid->out + signal_V->basic->omiga0) * signal_V->basic->Ts;
    signal_V->theta = (float)fmod(signal_V->theta, 2 * PI);
}

/**
 * @brief 电流内环控制
 * @param signal_I 电流信号指针
 * @param signal_V 电压信号指针
 * @param Iset 电流设定值(有效值)
 * @param PF 功率因数
 */
void single_Phase_Loop_I(single_Phase_Signal_I *signal_I, single_Phase_Signal_V *signal_V, float Iset, float PF)
{
    /* 对信号先进行sogi变换，得到两个相位相差90度的信号 */
    pll_Sogi(signal_I->basic->sogi, signal_I->basic->input);

    /* 在电压的系上得出电流的dq值 */
    float sinTheta = arm_sin_f32(signal_V->theta);
    float cosTheta = arm_cos_f32(signal_V->theta);

    arm_park_f32(signal_I->basic->sogi->alpha[0], signal_I->basic->sogi->beta[0], &signal_I->basic->park_d, &signal_I->basic->park_q, sinTheta, cosTheta);

    /* PI控制 */
    float PFTheta = asinf(PF);

    float Ipeak = Iset * 1.414f;

    float Ivalue = Ipeak * arm_sin_f32(PFTheta); /* 有功分量 */
    pid_positional(signal_I->pid_d, Ivalue, signal_I->basic->park_d);

    float Iphase = Ipeak * arm_cos_f32(PFTheta) * (signal_I->CorL ? 1 : -1); /* 无功分量 */
    pid_positional(signal_I->pid_q, Iphase, signal_I->basic->park_q);

    /* 解耦调制 */
    float Uabd = signal_V->basic->park_d - signal_I->pid_d->out + signal_I->basic->park_q * signal_I->basic->omiga0 * signal_I->L;
    float Uabq = signal_V->basic->park_q - signal_I->pid_q->out - signal_I->basic->park_d * signal_I->basic->omiga0 * signal_I->L;

    /* park逆变换 */
    arm_inv_park_f32(Uabd, Uabq, &signal_I->park_inv_alpha, &signal_I->park_inv_beta, sinTheta, cosTheta);

    /* 限幅 */
    signal_I->park_inv_alpha = fmaxf(-1.0f, fminf(1.0f, signal_I->park_inv_alpha));
    signal_I->park_inv_beta = fmaxf(-1.0f, fminf(1.0f, signal_I->park_inv_beta));
}

/**
 * @brief Sogi变换
 * @param sogi sogi指针
 * @param input 输入信号
 */
static void pll_Sogi(Sogi *sogi, float *input)
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
 * @brief 释放内存
 * @param signal 信号指针
 */
void single_Phase_Free_V(single_Phase_Signal_V *signal)
{
    free(signal->basic->sogi);
    free(signal->basic);
    free(signal->pid);
    free(signal);
}

/**
 * @brief 释放内存
 * @param signal 信号指针
 */
void single_Phase_Free_I(single_Phase_Signal_I *signal)
{
    free(signal->basic->sogi);
    free(signal->basic);
    free(signal->pid_d);
    free(signal->pid_q);
    free(signal);
}
