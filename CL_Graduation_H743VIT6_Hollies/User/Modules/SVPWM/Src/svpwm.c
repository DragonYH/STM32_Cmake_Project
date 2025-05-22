#include "svpwm.h"
#include "tim.h"
#include "dac.h"
#include "user_global.h"
#include "math.h"

static uint8_t Sector_Detection(float X, float Y, float Z);
static void Duty_Calculation(float Ta, float Tb, float Tc, float Ts);

/**
 * @brief  SVPWM控制
 * @param  Ualpha 整流时输入signal->park_inv_alpha 逆变时输入signal->basic->clarke_alpha
 * @param  Ubeta 整流时输入signal->park_inv_beta 逆变时输入signal->basic->clarke_beta
 */
void svpwm_Control(float Ualpha, float Ubeta, float Ts)
{
    // 计算中间变量
    Ualpha = M * Ualpha / Ubase; // 标幺化Clarke变换后的α轴电压
    Ubeta = M * Ubeta / Ubase;   // 标幺化Clarke变换后的β轴电压

    float Ualpha_ = 1.7320508f * Ualpha * Ts;
    float Ubeta_ = Ubeta * Ts;

    // 计算XYZ
    float X = Ubeta_;
    float Y = 0.5f * Ualpha_ + 0.5f * Ubeta_;
    float Z = 0.5f * Ubeta_ - 0.5f * Ualpha_;

    // 通过XYZ做扇区判断
    uint8_t sector = Sector_Detection(X, Y, Z);

    // 计算每一周期对应扇区各相的占用时间
    float Ta = 0, Tb = 0, Tc = 0;
    switch (sector)
    {
    case 1:
        Ta = Ts / 2.f + (-Z + X) / 2.f;
        Tb = Ta + Z;
        Tc = Tb - X;
        break;
    case 2:
        Tb = Ts / 2.f + (Z + Y) / 2.f;
        Ta = Tb - Z;
        Tc = Ta - Y;
        break;
    case 3:
        Tb = Ts / 2.f + (X - Y) / 2.f;
        Tc = Tb - X;
        Ta = Tc + Y;
        break;
    case 4:
        Tc = Ts / 2.f + (-X + Z) / 2.f;
        Tb = Tc + X;
        Ta = Tb - Z;
        break;
    case 5:
        Tc = Ts / 2.f + (-Y - Z) / 2.f;
        Ta = Tc + Y;
        Tb = Ta + Z;
        break;
    case 6:
        Ta = Ts / 2.f + (Y - X) / 2.f;
        Tc = Ta - Y;
        Tb = Tc + X;
        break;
    }

    Duty_Calculation(Ta, Tb, Tc, Ts);
}

/**
 * @brief  扇区判断
 */
static uint8_t Sector_Detection(float X, float Y, float Z)
{
    uint8_t sector = 0;
    if (Y < 0)
    {
        if (Z < 0)
            sector = 5;
        else
            sector = (X < 0) ? 4 : 3;
    }
    else
    {
        if (Z > 0)
            sector = 2;
        else
            sector = (X < 0) ? 6 : 1;
    }
    return sector;
}

/**
 * @brief  计算并输出占空比
 */
static void Duty_Calculation(float Ta, float Tb, float Tc, float Ts)
{
    // 计算占空比并更新TIM寄存器
    uint32_t ccr[3] = {
        (uint32_t)(Ta / Ts * TIM_PERIOD),
        (uint32_t)(Tb / Ts * TIM_PERIOD),
        (uint32_t)(Tc / Ts * TIM_PERIOD)};

    // 限制占空比在0到5999之间
    for (int i = 0; i < 3; ++i)
    {
        ccr[i] = fminf(TIM_PERIOD, fmaxf(50, ccr[i]));
    }

    // 更新TIM寄存器
    TIM1->CCR1 = ccr[0];
    TIM1->CCR2 = ccr[1];
    TIM1->CCR3 = ccr[2];
}
