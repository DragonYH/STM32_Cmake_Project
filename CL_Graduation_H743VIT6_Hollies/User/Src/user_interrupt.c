#include "user_global.h"
#include "user_interrupt.h"
#include "main.h"
#include "ad7606.h"
#include "tim.h"
#include "spi.h"
#include "arm_math.h"
#include "svpwm.h"
#include "three_phase_rectifier.h"
#include "user_task.h"

static void getVoltageCurrent(void);
static void calcEffectiveValue(void);

/**
 * @brief    外部中断
 * @param    GPIO_Pin
 * @return
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == AD7606_BUSY_Pin)
    {
        getVoltageCurrent();  // 获取电压电流
        calcEffectiveValue(); // 计算有效值

        if (runState == RUN)
        {
            switch (inputMode)
            {
            case DC:
                if (outputMode == AC_SINGLE)
                {
                }
                else if (outputMode == AC_THREE)
                {
                    three_Phase_PLL_V(signal_V);                                                                     // 电压锁相
                    svpwm_Control(signal_V->basic->clarke_alpha, signal_V->basic->clarke_beta, signal_V->basic->Ts); // SVPWM控制
                }
                break;
            case AC_SINGLE:
                break;
            case AC_THREE:
                break;
            }
        }
    }
}

/**
 * @brief  获取相电压和电流
 */
static void getVoltageCurrent(void)
{
    // 读取AD7606数据
    float adcValue[8] = {0};
    ad7606_GetValue(&hspi2, 8, adcValue);

    // 处理电压数据，将线电压转为相电压
    float Uab = adcValue[1];
    float Ubc = adcValue[3];
    float Uca = adcValue[5];

    signal_V->basic->input_a = (Uab - Uca) / 3.f;
    signal_V->basic->input_b = (Ubc - Uab) / 3.f;
    signal_V->basic->input_c = (Uca - Ubc) / 3.f;

    // 处理电流数据
    signal_I->basic->input_a = adcValue[0] * 2.178571f;
    signal_I->basic->input_b = adcValue[2] * 2.250774f;
    signal_I->basic->input_c = adcValue[4] * 2.172956f;
}

/**
 * @brief  计算电压电流有效值
 */
static void calcEffectiveValue(void)
{
    static uint16_t cnt = 0;

    static float V_rms_a[400] = {0};
    static float V_rms_b[400] = {0};
    static float V_rms_c[400] = {0};
    static float I_rms_a[400] = {0};
    static float I_rms_b[400] = {0};
    static float I_rms_c[400] = {0};

    V_rms_a[cnt] = signal_V->basic->input_a;
    V_rms_b[cnt] = signal_V->basic->input_b;
    V_rms_c[cnt] = signal_V->basic->input_c;
    I_rms_a[cnt] = signal_I->basic->input_a;
    I_rms_b[cnt] = signal_I->basic->input_b;
    I_rms_c[cnt] = signal_I->basic->input_c;

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
}
