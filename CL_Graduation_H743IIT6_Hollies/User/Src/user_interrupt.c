#include "user_interrupt.h"
#include "main.h"
#include "ad7606.h"
#include "tim.h"
#include "spi.h"
#include "user_global.h"

/**
 * @brief    外部中断
 * @param    GPIO_Pin
 * @return
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == AD7606_BUSY_Pin)
    {
        // 读取AD7606数据
        float adcValue[8] = {0};
        ad7606_GetValue(&hspi2, 8, adcValue);

        float Uab = adcValue[2];
        float Ubc = adcValue[4];
        float Uca = adcValue[6];
        signal_V->basic->input_a = Uab;
        signal_V->basic->input_b = Ubc;
        signal_V->basic->input_c = Uca;
    }
}
