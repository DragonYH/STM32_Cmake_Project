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

#define enableDrive() HAL_GPIO_WritePin(DIS_GPIO_Port, DIS_Pin, GPIO_PIN_RESET) // 使能驱动
#define disableDrive() HAL_GPIO_WritePin(DIS_GPIO_Port, DIS_Pin, GPIO_PIN_SET)  // 禁用驱动

char chipTemp[8] = {0};
float mcuTemperature = 0.0f;

void StartDefaultTask(void *argument)
{
    /* USER CODE BEGIN StartDefaultTask */
    UNUSED(argument);
    /* Infinite loop */
    for (;;)
    {
        // if (runState == RUN)
        // {
        //     HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        //     osDelay(100);
        //     HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        //     osDelay(200);
        //     HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        //     osDelay(100);
        //     HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        //     osDelay(200);
        //     HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
        // }
        // else if (runState == STOP)
        // {
        //     HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
        // }
        // else
        // {
        //     HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
        // }

        // 判断输入模式
        osDelay(1000);
        if (signal_V->basic->rms_a > 10.f && signal_V->basic->rms_b > 10.f && signal_V->basic->rms_c > 10.f && dcVolt < 4.f)
        {
            inputMode = AC_THREE;
            outputMode = DC;
        }
        else if (signal_V->basic->rms_a > 10.f && dcVolt < 4.f)
        {
            inputMode = AC_SINGLE;
            outputMode = DC;
        }
        else if (signal_V->basic->rms_a < 4.f && dcVolt > 10.f)
        {
            inputMode = DC;
            outputMode = AC_THREE;
        }
        vTaskDelete(NULL); // 删除当前任务
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
            double temperature = HAL_ADC_GetValue(&hadc3) / 3.3f * 2.5f; /* 读出转换结果 */
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
        OLED_ShowString(0, 24, (uint8_t *)"AC  V(V)  C(A)", 12);

        memset(temp, 0, sizeof(temp));
        snprintf(temp, sizeof(temp), "A   %-5.2f %-4.2f", signal_V->basic->rms_a, signal_I->basic->rms_a);
        OLED_ShowString(0, 36, (uint8_t *)temp, 12);

        memset(temp, 0, sizeof(temp));
        snprintf(temp, sizeof(temp), "B   %-5.2f %-4.2f", signal_V->basic->rms_b, signal_I->basic->rms_b);
        OLED_ShowString(0, 48, (uint8_t *)temp, 12);

        memset(temp, 0, sizeof(temp));
        snprintf(temp, sizeof(temp), "C   %-5.2f %-4.2f", signal_V->basic->rms_c, signal_I->basic->rms_c);
        OLED_ShowString(0, 60, (uint8_t *)temp, 12);
        // 显示输出电压和电流
        memset(temp, 0, sizeof(temp));
        OLED_ShowString(0, 72, (uint8_t *)"DC  V(V)  C(A)", 12);
        snprintf(temp, sizeof(temp), "    %-5.2f %-4.2f", dcVolt, dcCurr);
        OLED_ShowString(0, 84, (uint8_t *)temp, 12);

        // 调试
        memset(temp, 0, sizeof(temp));
        snprintf(temp, sizeof(temp), "%6.2f", dcMedVolt);
        OLED_ShowString(86, 72, (uint8_t *)temp, 12);
        memset(temp, 0, sizeof(temp));
        snprintf(temp, sizeof(temp), "%6.2f", dcMedCurr);
        OLED_ShowString(86, 84, (uint8_t *)temp, 12);

        memset(temp, 0, sizeof(temp));
        snprintf(temp, sizeof(temp), "%6.2f", signal_V_single->basic->sogi->alpha[0]);
        OLED_ShowString(86, 60, (uint8_t *)temp, 12);

        memset(temp, 0, sizeof(temp));
        snprintf(temp, sizeof(temp), "%6.2f", signal_V->basic->clarke_alpha);
        OLED_ShowString(86, 48, (uint8_t *)temp, 12);

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
        if (isKEY0 || isKEY1 || isKEY2 || isKEY3)
        {
            osDelay(100); /* 消抖 */
            if (isKEY3)   // 切换运行状态
            {
                while (isKEY3)
                    ;
                runState = (runState == RUN) ? STOP : RUN;
            }
            else if (isKEY2) // 切换设置模式
            {
                osDelay(500);
                if (isKEY2) // 如果长按则切换输出模式
                {
                    if (inputMode == DC)
                        outputMode = (outputMode == AC_SINGLE) ? AC_THREE : AC_SINGLE;
                    while (isKEY2)
                        ;
                }
                else // 短按切换设置模式
                {
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
            }
            else if (isKEY1) // 增加数值
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
            else if (isKEY0) // 减小数值
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
    INA228_config(INA228_0);       /* 配置INA228_0传感器 */
    INA228_config(INA228_1);       /* 配置INA228_1传感器 */
    HAL_TIM_Base_Start_IT(&htim2); /* 启动定时器2 */
    /* Infinite loop */
    for (;;)
    {

        osDelay(1000);
    }
    /* USER CODE END StartDCSampingTask */
}

void StartMedDCControl(void *argument)
{
    /* USER CODE BEGIN StartMedDCControl */
    UNUSED(argument);

    PID dcPid;
    pid_Init(&dcPid, 40.f, 10.f, 0.2f, 11879.f, 1999.f); // PID参数初始化
    dcPid.out = 11879.0f;

    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);    // 启动PWM输出
    HAL_TIMEx_PWMN_Start(&htim8, TIM_CHANNEL_1); // 启动PWM输出

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);

    /* Infinite loop */
    for (;;)
    {
        switch (runState)
        {
        case RUN:
            if (inputMode == DC)
            {
                if (outputMode == AC_SINGLE)
                {
                    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
                    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);
                    pid_incremental(&dcPid, signal_V->basic->rms_a, setVolt); // PID计算
                }
                else if (outputMode == AC_THREE)
                {
                    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
                    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
                    pid_incremental(&dcPid, signal_V->basic->rms_a, setVolt); // PID计算
                }
                else
                {
                    runState = FAULT; // 运行状态异常
                    break;
                }

                __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, dcPid.out); // 设置PWM占空比
            }
            else if (inputMode == AC_SINGLE || inputMode == AC_THREE)
            {
                pid_incremental(&dcPid, dcMedVolt, signal_V->basic->rms_a * 1.8f); // PID计算
                float tmp = dcPid.out / 11879.f;
                loopI = tmp * 4.f;
            }
            else
            {
                runState = FAULT; // 运行状态异常
                break;
            }
            enableDrive(); // 使能MOSFET
            break;
        case STOP:
            disableDrive(); // 禁用MOSFET
            dcPid.out = 11879.0f;
            break;
        case FAULT:
            disableDrive(); // 禁用MOSFET
            break;
        default:
            disableDrive(); // 禁用MOSFET
            break;
        }
        osDelay(10);
    }
    /* USER CODE END StartMedDCControl */
}

void StartDCControlTask(void *argument)
{
    /* USER CODE BEGIN StartDCControlTask */
    UNUSED(argument);
    PID dcPid;
    pid_Init(&dcPid, 20.f, 1.f, 0.2f, 11879.f, 3999.f); // PID参数初始化
    dcPid.out = 5999.f;                                 // 设置初始PWM占空比
    /* Infinite loop */
    for (;;)
    {
        if (outputMode == DC && runState == RUN)
        {
            pid_incremental(&dcPid, setVolt, dcVolt);                // PID计算
            __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, dcPid.out); // 设置PWM占空比
        }
        osDelay(10);
    }
    /* USER CODE END StartDCControlTask */
}

static void userFuncGetChipTemperature(void)
{
    shellPrint(&shell, "Temperature: %-4s\r\n", chipTemp); /* 打印芯片温度 */
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), getChipTemperature, userFuncGetChipTemperature, 获取芯片温度);
