#ifndef PLL_H
#define PLL_H

#include "pid.h"
#include "main.h"

/* sogi结构体 */
typedef struct pll_Sogi_t
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
} pll_Sogi_t;

typedef struct pll_Basic_t
{
    /* 基本变量 */
    float input[3]; /* a相输入 */
    float rms;      /* a相有效值 */

    /* sogi变换相关变量 */
    pll_Sogi_t *sogi;

    /* park变换相关变量 */
    float park_d; /* 有功分量 */
    float park_q; /* 无功分量 */

    /* 配置参数 */
    float omiga0; /* 无阻尼自然频率，2*pi*频率 */
    float Ts;     /* 采样周期 */

} pll_Basic_t;

typedef struct pll_Signal_t
{
    pll_Basic_t *basic;
    PID_t *pid;
    float theta;
} pll_Signal_t;

void pll_Init(pll_Signal_t **signal, float f, uint16_t F);
void pll_Control(pll_Signal_t *signal);

#endif
