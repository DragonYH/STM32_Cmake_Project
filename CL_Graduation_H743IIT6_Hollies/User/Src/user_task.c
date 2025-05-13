#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "shell.h"
#include "user_shell.h"
#include "user_task.h"
#include "usbd_cdc_if.h"
#include <stdio.h>
#include "adc.h"
#include "oled.h"
#include "user_global.h"
#include "tim.h"
#include "i2c.h"
#include "ina228.h"

#define isKEY0 (HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == GPIO_PIN_RESET) // 按键0
#define isKEY1 (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET) // 按键1
#define isKEY2 (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET) // 按键2
#define isKEY3 (HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET) // 按键3
#define isKEY4 (HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin) == GPIO_PIN_RESET) // 按键4

char chipTemp[8] = {0};
float mcuTemperature = 0.0f;
float dcMedVolt = 0.0f; // 中间直流电压
float dcMedCurr = 0.0f; // 中间直流电流

void StartDefaultTask(void *argument)
{
    /* USER CODE BEGIN StartDefaultTask */
    UNUSED(argument);
    /* Infinite loop */
    for (;;)
    {
        if (runState == RUN)
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
        }
        else if (runState == STOP)
        {
            HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
        }
        else
        {
            HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
        }
        osDelay(1000);
    }
    /* USER CODE END StartDefaultTask */
}

void StartChipTemperatureTask(void *argument)
{
    /* USER CODE BEGIN StartChipTemperatureTask */
    UNUSED(argument);
    osDelay(500);
    HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);
    /* Infinite loop */
    for (;;)
    {
        HAL_ADC_Start(&hadc3);
        if (HAL_ADC_PollForConversion(&hadc3, 1000) == HAL_OK) /* 判断是否转换完成 */
        {
            uint16_t temperature = HAL_ADC_GetValue(&hadc3); /* 读出转换结果 */
            mcuTemperature = ((110.0f - 30.0f) / (*(unsigned short *)(0x1FF1E840) - *(unsigned short *)(0x1FF1E820))) * (temperature - *(unsigned short *)(0x1FF1E820)) + 30.f;

            snprintf(chipTemp, sizeof(chipTemp), "%4.1f", mcuTemperature); /* 将温度值转换为字符串 */
        }
        else
        {
            snprintf(chipTemp, sizeof(chipTemp), "Error");
        }

        osDelay(500);
    }
}

void StartOledDisplayTask(void *argument)
{
    /* USER CODE BEGIN StartOLEDDisplayTask */
    UNUSED(argument);
    OLED_Init();
    /* Infinite loop */
    for (;;)
    {
        char temp[32] = {0};
        char runState_s[6] = {0};
        char inputMode_s[4] = {0};
        char outputMode_s[4] = {0};
        char setMode_s[3] = {0};

        switch (runState)
        {
        case STOP:
            snprintf(runState_s, sizeof(runState_s), "STOP ");
            break;
        case RUN:
            snprintf(runState_s, sizeof(runState_s), "RUN  ");
            break;
        default:
            snprintf(runState_s, sizeof(runState_s), "FAULT");
            break;
        }

        switch (inputMode)
        {
        case DC:
            snprintf(inputMode_s, sizeof(inputMode_s), "DC ");
            break;
        case AC_SINGLE:
            snprintf(inputMode_s, sizeof(inputMode_s), "AC1");
            break;
        case AC_THREE:
            snprintf(inputMode_s, sizeof(inputMode_s), "AC3");
            break;
        default:
            snprintf(inputMode_s, sizeof(inputMode_s), "0");
            break;
        }

        switch (outputMode)
        {
        case DC:
            snprintf(outputMode_s, sizeof(outputMode_s), "DC ");
            break;
        case AC_SINGLE:
            snprintf(outputMode_s, sizeof(outputMode_s), "AC1");
            break;
        case AC_THREE:
            snprintf(outputMode_s, sizeof(outputMode_s), "AC3");
            break;
        default:
            snprintf(outputMode_s, sizeof(outputMode_s), "0");
            break;
        }

        switch (setMode)
        {
        case VOLT_SET:
            snprintf(setMode_s, sizeof(setMode_s), "V ");
            break;
        case CURR_SET:
            snprintf(setMode_s, sizeof(setMode_s), "I ");
            break;
        case PF_SET:
            snprintf(setMode_s, sizeof(setMode_s), "PF");
            break;
        default:
            snprintf(setMode_s, sizeof(setMode_s), "0");
            break;
        }
        // 显示标题栏
        snprintf(temp, sizeof(temp), "%-5s %-3s %-3s", runState_s, inputMode_s, outputMode_s);
        OLED_ShowString(0, 0, (uint8_t *)"State In  Out", 12);
        OLED_ShowString(0, 12, (uint8_t *)temp, 12);
        // 显示输入电压和电流
        memset(temp, 0, sizeof(temp));
        OLED_ShowString(0, 24, (uint8_t *)"In  V(V)  C(A)", 12);

        memset(temp, 0, sizeof(temp));
        snprintf(temp, sizeof(temp), "A   %-5.2f %-4.2f", signal_V->basic->input_a, signal_I->basic->input_a);
        OLED_ShowString(0, 36, (uint8_t *)temp, 12);

        memset(temp, 0, sizeof(temp));
        snprintf(temp, sizeof(temp), "B   %-5.2f %-4.2f", signal_V->basic->input_b, signal_I->basic->input_b);
        OLED_ShowString(0, 48, (uint8_t *)temp, 12);

        memset(temp, 0, sizeof(temp));
        snprintf(temp, sizeof(temp), "C   %-5.2f %-4.2f", signal_V->basic->input_c, signal_I->basic->input_c);
        OLED_ShowString(0, 60, (uint8_t *)temp, 12);
        // 显示输出电压和电流
        memset(temp, 0, sizeof(temp));
        OLED_ShowString(0, 72, (uint8_t *)"Out V(V)  C(A)", 12);
        snprintf(temp, sizeof(temp), "DC  %-5.2f %-4.2f", 0.f, 0.f);
        OLED_ShowString(0, 84, (uint8_t *)temp, 12);
        // 显示设置
        memset(temp, 0, sizeof(temp));
        OLED_ShowString(0, 96, (uint8_t *)"Set V(V)  C(A) PF", 12);
        snprintf(temp, sizeof(temp), "%s  %-5.2f %-4.2f %5.2f", setMode_s, setVolt, setCurr, setPF);
        OLED_ShowString(0, 108, (uint8_t *)temp, 12);
        // 显示温度
        memset(temp, 0, sizeof(temp));
        snprintf(temp, sizeof(temp), "%s", chipTemp);
        OLED_ShowString(98, 0, (uint8_t *)temp, 12);

        OLED_Refresh();
        osDelay(100);
    }
    /* USER CODE END StartOLEDDisplayTask */
}

void StartKeyTask(void *argument)
{
    /* USER CODE BEGIN StartKeyTask */
    UNUSED(argument);
    float maxVolt = 32.f; // 最大电压
    float maxCurr = 2.f;  // 最大电流
    float minVolt = 12.f; // 最小电压
    float minCurr = 0.1f; // 最小电流
    /* Infinite loop */
    for (;;)
    {
        /* 按键按下 */
        if (isKEY0 || isKEY2 || isKEY3 || isKEY4)
        {
            osDelay(50); /* 消抖 */
            if (isKEY0)  // 切换运行状态
            {
                while (isKEY0)
                    ;
                runState = (runState == RUN) ? STOP : RUN;
            }
            else if (isKEY2) // 切换设置模式
            {
                while (isKEY2)
                    ;
                switch (setMode)
                {
                case VOLT_SET:
                    setMode = CURR_SET;
                    break;
                case CURR_SET:
                    setMode = PF_SET;
                    break;
                case PF_SET:
                    setMode = VOLT_SET;
                    break;
                }
            }
            else if (isKEY3) // 增加数值
            {
                float tmp;
                switch (setMode)
                {
                case VOLT_SET:
                    if (setVolt < maxVolt)
                        setVolt += 0.1f;
                    break;
                case CURR_SET:
                    if (setCurr < maxCurr)
                        setCurr += 0.1f;
                    break;
                case PF_SET:
                    tmp = setPF;
                    if (tmp < 1.0f)
                        tmp += 0.01f;
                    else
                    {
                        tmp = -tmp;
                    }
                    if (tmp < 0 && tmp > -0.7f) // 限值
                        tmp = -0.7f;
                    setPF = tmp;
                    break;
                }
            }
            else if (isKEY4) // 减小数值
            {
                float tmp;
                switch (setMode)
                {
                case VOLT_SET:
                    if (setVolt > minVolt)
                        setVolt -= 0.1f;
                    break;
                case CURR_SET:
                    if (setCurr > minCurr)
                        setCurr -= 0.1f;
                    break;
                case PF_SET:
                    tmp = setPF;
                    if (tmp > -1.0f)
                        tmp -= 0.01f;
                    else
                    {
                        tmp = -tmp;
                    }
                    if (tmp > 0 && tmp < 0.7f) // 限值
                        tmp = 0.7f;
                    setPF = tmp;
                    break;
                }
            }
        }
        osDelay(10);
    }
    /* USER CODE END StartKeyTask */
}

void StartDCSampingTask(void *argument)
{
    /* USER CODE BEGIN StartDCSampingTask */
    UNUSED(argument);
    INA228_config(INA228_0); /* 配置INA228_0传感器 */
    INA228_config(INA228_1); /* 配置INA228_1传感器 */
    /* Infinite loop */
    for (;;)
    {
        dcVolt = INA228_getVBUS_V(INA228_0);       /* 获取直流电压 */
        dcCurr = INA228_getCURRENT_A(INA228_0);    /* 获取直流电流 */
        dcMedVolt = INA228_getVBUS_V(INA228_1);    /* 获取中间直流电压 */
        dcMedCurr = INA228_getCURRENT_A(INA228_1); /* 获取中间直流电流 */
        osDelay(5);
    }
    /* USER CODE END StartDCSampingTask */
}

static void userFuncGetChipTemperature(void)
{
    shellPrint(&shell, "Temperature: %-4s\r\n", chipTemp); /* 打印芯片温度 */
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), getChipTemperature, userFuncGetChipTemperature, 获取芯片温度);
