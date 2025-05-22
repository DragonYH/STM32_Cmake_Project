#include "user_global.h"
#include "user_interrupt.h"
#include "main.h"
#include "ad7606.h"
#include "tim.h"
#include "spi.h"
#include "arm_math.h"
#include "svpwm.h"
#include "three_phase_rectifier.h"
#include "single_phase_rectifier.h"
#include "user_task.h"

static void three_Phase_getVoltageCurrent(void);
static void three_Phase_simulateInput(uint16_t cnt);
static void single_Phase_simulateInput(uint16_t cnt);
static void spwm_Control(float Ualpha);

/**
 * @brief    外部中断
 * @param    GPIO_Pin
 * @return
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == AD7606_BUSY_Pin)
    {
        three_Phase_getVoltageCurrent(); // 获取电压电流

        if (runState == RUN)
        {
            switch (inputMode)
            {
            case DC:
                if (outputMode == AC_SINGLE)
                {
                    single_Phase_PLL_V(signal_V_single);                  // 电压锁相
                    spwm_Control(signal_V_single->basic->sogi->alpha[0]); // SPWM控制
                }
                else if (outputMode == AC_THREE)
                {
                    three_Phase_PLL_V(signal_V);                                                                     // 电压锁相
                    svpwm_Control(signal_V->basic->clarke_alpha, signal_V->basic->clarke_beta, signal_V->basic->Ts); // SVPWM控制
                }
                break;
            case AC_SINGLE:
                single_Phase_PLL_V(signal_V_single);                                 // 电压锁相
                single_Phase_Loop_I(signal_I_single, signal_V_single, loopI, setPF); // 电流控制
                spwm_Control(signal_I_single->park_inv_alpha);
                break;
            case AC_THREE:
                three_Phase_PLL_V(signal_V);                                                           // 电压锁相
                three_Phase_Loop_I(signal_I, signal_V, loopI, setPF);                                  // 电流控制
                svpwm_Control(signal_I->park_inv_alpha, signal_I->park_inv_beta, signal_V->basic->Ts); // SVPWM控制
                break;
            case INIT:
                break;
            }
        }
    }
}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM6 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* USER CODE BEGIN Callback 0 */

    /* USER CODE END Callback 0 */
    if (htim->Instance == TIM6)
    {
        HAL_IncTick();
    }
    if (htim->Instance == TIM2)
    {
        dcVolt = INA228_getVBUS_V(INA228_0);       /* 获取直流电压 */
        dcCurr = INA228_getCURRENT_A(INA228_0);    /* 获取直流电流 */
        dcMedVolt = INA228_getVBUS_V(INA228_1);    /* 获取中间直流电压 */
        dcMedCurr = INA228_getCURRENT_A(INA228_1); /* 获取中间直流电流 */
    }
    /* USER CODE BEGIN Callback 1 */

    /* USER CODE END Callback 1 */
}

/**
 * @brief  获取三相相电压和电流
 */
static void three_Phase_getVoltageCurrent(void)
{
    // 读取AD7606数据
    float adcValue[8] = {0};
    ad7606_GetValue(&hspi2, 8, adcValue);

    // 处理电压数据，将线电压转为相电压
    float Uab = adcValue[1] * 14.92281f;
    float Ubc = adcValue[3] * 16.10003f;
    float Uca = adcValue[5] * 14.57933f;

    // 处理电流数据
    signal_I->basic->input_a = adcValue[0] * 1.95961f;
    signal_I->basic->input_b = adcValue[2] * 1.95316f;
    signal_I->basic->input_c = adcValue[4] * 1.95471f;

    static uint16_t cnt = 0;

    static float V_rms_a[400] = {0};
    static float V_rms_b[400] = {0};
    static float V_rms_c[400] = {0};
    static float I_rms_a[400] = {0};
    static float I_rms_b[400] = {0};
    static float I_rms_c[400] = {0};

    if (inputMode == AC_THREE || outputMode == AC_THREE)
    {
        V_rms_a[cnt] = Uab;
        V_rms_b[cnt] = Ubc;
        V_rms_c[cnt] = Uca;
        I_rms_a[cnt] = signal_I->basic->input_a;
        I_rms_b[cnt] = signal_I->basic->input_b;
        I_rms_c[cnt] = signal_I->basic->input_c;
    }
    else if (inputMode == AC_SINGLE || outputMode == AC_SINGLE)
    {
        signal_V_single->basic->input[0] = Uab;
        V_rms_a[cnt] = signal_V_single->basic->input[0];
        V_rms_b[cnt] = 0.f;
        V_rms_c[cnt] = 0.f;
        I_rms_a[cnt] = signal_I->basic->input_a;
        I_rms_b[cnt] = 0.f;
        I_rms_c[cnt] = 0.f;
    }

    if (++cnt == 400)
    {
        cnt = 0;
        arm_rms_f32(V_rms_a, 400, &signal_V->basic->rms_a);
        arm_rms_f32(V_rms_b, 400, &signal_V->basic->rms_b);
        arm_rms_f32(V_rms_c, 400, &signal_V->basic->rms_c);
        arm_rms_f32(I_rms_a, 400, &signal_I->basic->rms_a);
        arm_rms_f32(I_rms_b, 400, &signal_I->basic->rms_b);
        arm_rms_f32(I_rms_c, 400, &signal_I->basic->rms_c);
    }

    if (inputMode == AC_THREE || outputMode == AC_THREE)
    {
        signal_V->basic->input_a = (Uab - Uca) / 3.f;
        signal_V->basic->input_b = (Ubc - Uab) / 3.f;
        signal_V->basic->input_c = (Uca - Ubc) / 3.f;
    }

    // 模拟输入
    if (outputMode == AC_THREE)
    {
        three_Phase_simulateInput(cnt);
    }
    else if (outputMode == AC_SINGLE)
    {
        single_Phase_simulateInput(cnt);
    }
}

/**
 * @brief  模拟三相电压输入
 */
static void three_Phase_simulateInput(uint16_t cnt)
{
    signal_V->basic->input_a = 35.f * arm_sin_f32(cnt * 2 * PI / 400);
    signal_V->basic->input_b = 35.f * arm_sin_f32(cnt * 2 * PI / 400 - 2 * PI / 3);
    signal_V->basic->input_c = 35.f * arm_sin_f32(cnt * 2 * PI / 400 - 4 * PI / 3);
}
/**
 * @brief  模拟单相电压输入
 */
static void single_Phase_simulateInput(uint16_t cnt)
{
    signal_V_single->basic->input[0] = 35.f * arm_sin_f32(cnt * 2 * PI / 400);
}

static void spwm_Control(float Ualpha)
{
    float compare = Ualpha / Ubase * (TIM_PERIOD - 1);
    if (compare > TIM_PERIOD)
    {
        compare = TIM_PERIOD;
    }
    else if (compare < -TIM_PERIOD)
    {
        compare = -TIM_PERIOD;
    }
    if (Ualpha > 0)
    {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, compare); // 设置PWM占空比
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);       // 设置PWM占空比
    }
    else
    {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);        // 设置PWM占空比
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, -compare); // 设置PWM占空比
    }
}
