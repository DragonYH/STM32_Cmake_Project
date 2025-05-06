#ifndef __THREE_PHRASE_PLL_H
#define __THREE_PHRASE_PLL_H

#include "pid.h"
#include "main.h"

typedef struct three_Phase_Signal_Basic
{
    // 基本变量
    float input_a; // a相输入
    float input_b; // b相输入
    float input_c; // c相输入
    float rms_a;   // a相有效值
    float rms_b;   // b相有效值
    float rms_c;   // c相有效值
    // clarke变换相关变量
    float clarke_alpha; // alpha分量
    float clarke_beta;  // beta分量
    // park变换相关变量
    float park_d; // 有功分量
    float park_q; // 无功分量
    // 配置参数
    float omiga0; // 无阻尼自然频率，2*pi*频率
    float Ts;     // 采样周期

} three_Phase_Signal_Basic;

// 电压信号数据
typedef struct three_Phase_Signal_V
{
    // 基本变量
    three_Phase_Signal_Basic *basic;
    float theta; // 当前角度
    // 控制参数
    PID *pid; // 锁相pid指针
} three_Phase_Signal_V;
// 电流信号数据
typedef struct three_Phase_Signal_I
{
    // 基本变量
    three_Phase_Signal_Basic *basic;
    // park逆变换相关变量
    float park_inv_alpha; // 逆变换后的alpha
    float park_inv_beta;  // 逆变换后的beta
    // 控制参数
    uint8_t CorL; // 0:感性 1:容性
    float L;      // 电感
    PID *pid_d;   // 控制电流最大值pi指针
    PID *pid_q;   // 控制相位pi指针
} three_Phase_Signal_I;

void three_Phase_Init_V(three_Phase_Signal_V **signal, float f, uint16_t F);
void three_Phase_PLL_V(three_Phase_Signal_V *signal_V);
void three_Phase_Init_I(three_Phase_Signal_I **signal, float f, uint16_t F);
void three_Phase_Loop_I(three_Phase_Signal_I *signal_I, three_Phase_Signal_V *signal_V, float Iset, float PF);
void three_Phase_Free_V(three_Phase_Signal_V *signal);
void three_Phase_Free_I(three_Phase_Signal_I *signal);

#endif
