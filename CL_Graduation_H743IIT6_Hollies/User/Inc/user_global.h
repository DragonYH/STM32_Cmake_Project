#ifndef __USER_GLOBAL_H
#define __USER_GLOBAL_H

#include "ina228.h"
#include "three_phase_rectifier.h"

// 基准值
#define Ubase (36.f * 1.414f) // 电压基准
#define Ibase (6.f * 1.414f)  // 电流基准

// 保护阈值
#define protection_Udc (90.f) // 直流电压保护阈值
#define protection_Idc (10.f) // 直流电流保护阈值
#define protection_Uac (30.f) // 交流电压保护阈值
#define protection_Iac (10.f) // 交流电流保护阈值

// 全局变量声明
extern const INA228_Handle INA228_0;
extern float U;
extern float I;
extern float Utarget;
extern float Iref;
extern float M;
extern pll_Signal_V *signal_V;
extern pll_Signal_I *signal_I;

// 运行状态枚举
enum run_state
{
    STOP = 0,
    RUN = 1,
    FAULT = 2
};
extern volatile enum run_state runState;

// 模式定义
enum input_output_mode
{
    DC = 0,        // 直流
    AC_SINGAL = 1, // 单相交流
    AC_THREE = 2   // 三相交流
};
extern volatile enum input_output_mode inputMode;  // 输入模式
extern volatile enum input_output_mode outputMode; // 输出模式

enum run_mode
{
    VOLT_STAB = 0, // 电压稳定模式
    CURR_STAB = 1, // 电流稳定模式
};
extern volatile enum run_mode runMode; // 运行模式

#endif // __USER_GLOBAL_H
