#ifndef __THREE_PHASE_PLL_H
#define __THREE_PHASE_PLL_H

#include "pid.h"
#include "main.h"

/* sogi结构体 */
typedef struct Sogi
{
    float alpha[3]; /* 输出序列alpha */
    float beta[3];  /* 输出序列beta	滞后90度于序列alpha */

    /* 中间变量 */
    float k; /* 阻尼比 典型值1.41 */
    float lamda;
    float x;
    float y;
    float b0;
    float a1;
    float a2;
} Sogi;

typedef struct single_Phase_Signal_Basic
{
    /* 基本变量 */
    float input[3]; /* a相输入 */
    float rms;      /* a相有效值 */

    /* sogi变换相关变量 */
    Sogi *sogi;

    /* park变换相关变量 */
    float park_d; /* 有功分量 */
    float park_q; /* 无功分量 */

    /* 配置参数 */
    float omiga0; /* 无阻尼自然频率，2*pi*频率 */
    float Ts;     /* 采样周期 */

} single_Phase_Signal_Basic;

/* 电压信号数据 */
typedef struct single_Phase_Signal_V
{
    /* 基本变量 */
    single_Phase_Signal_Basic *basic;

    float theta; /* 当前角度 */

    /* 控制参数 */
    PID *pid; /* 锁相pid指针 */
} single_Phase_Signal_V;

/* 电流信号数据 */
typedef struct single_Phase_Signal_I
{
    /* 基本变量 */
    single_Phase_Signal_Basic *basic;

    /* park逆变换相关变量 */
    float park_inv_alpha; /* 逆变换后的alpha */
    float park_inv_beta;  /* 逆变换后的beta */

    /* 控制参数 */
    uint8_t CorL; /* 0:感性 1:容性 */
    float L;      /* 电感 */
    PID *pid_d;   /* 控制电流最大值pi指针 */
    PID *pid_q;   /* 控制相位pi指针 */
} single_Phase_Signal_I;

void single_Phase_Init_V(single_Phase_Signal_V **signal, float f, uint16_t F);
void single_Phase_PLL_V(single_Phase_Signal_V *signal_V);
void single_Phase_Init_I(single_Phase_Signal_I **signal, float f, uint16_t F);
void single_Phase_Loop_I(single_Phase_Signal_I *signal_I, single_Phase_Signal_V *signal_V, float Iset, float PF);
void single_Phase_Free_V(single_Phase_Signal_V *signal);
void single_Phase_Free_I(single_Phase_Signal_I *signal);

#endif
