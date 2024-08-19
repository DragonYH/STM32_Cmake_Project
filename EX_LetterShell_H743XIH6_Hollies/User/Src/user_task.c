#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "shell.h"
#include "user_shell.h"
#include "user_task.h"
#include "usbd_cdc_if.h"
#include "stdio.h"
#include "adc.h"

char chipTemp[16] = {0};
/* 注册为输出变量 */
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_VAR_STRING), chipTemp, chipTemp, 芯片温度);

void StartDefaultTask(void *argument)
{
    /* USER CODE BEGIN StartDefaultTask */
    UNUSED(argument);
    /* Infinite loop */
    for (;;)
    {
        HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
        osDelay(100);
        HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
        osDelay(200);
        HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
        osDelay(100);
        HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
        osDelay(200);
        HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
        osDelay(1000);
    }
    /* USER CODE END StartDefaultTask */
}

void StartChipTemperatureTask(void *argument)
{
    /* USER CODE BEGIN StartChipTemperatureTask */
    UNUSED(argument);
    osDelay(500);
    HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);
    /* Infinite loop */
    for (;;)
    {
        HAL_ADC_Start(&hadc3);
        if (HAL_ADC_PollForConversion(&hadc3, 1000) == HAL_OK) /* 判断是否转换完成 */
        {
            uint16_t temperature = HAL_ADC_GetValue(&hadc3); /* 读出转换结果 */
            float mcuTemperature = ((110.0f - 30.0f) / (*(unsigned short *)(0x1FF1E840) - *(unsigned short *)(0x1FF1E820))) * (temperature - *(unsigned short *)(0x1FF1E820)) + 30.f;

            snprintf(chipTemp, sizeof(chipTemp), "%f", mcuTemperature); /* 将温度值转换为字符串 */
        }
        else
        {
            snprintf(chipTemp, sizeof(chipTemp), "Error");
        }

        osDelay(500);
    }
}
