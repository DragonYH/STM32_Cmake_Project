/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "user_task.h"
#include "shell.h"
#include "user_shell.h"
#include "ad7606.h"
#include "tim.h"
#include "three_phase_rectifier.h"
#include "user_global.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for letterShellTask */
osThreadId_t letterShellTaskHandle;
const osThreadAttr_t letterShellTask_attributes = {
  .name = "letterShellTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal7,
};
/* Definitions for chipTemperature */
osThreadId_t chipTemperatureHandle;
const osThreadAttr_t chipTemperature_attributes = {
  .name = "chipTemperature",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for oledDisplayTask */
osThreadId_t oledDisplayTaskHandle;
const osThreadAttr_t oledDisplayTask_attributes = {
  .name = "oledDisplayTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for keyTask */
osThreadId_t keyTaskHandle;
const osThreadAttr_t keyTask_attributes = {
  .name = "keyTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for dcSampingTask */
osThreadId_t dcSampingTaskHandle;
const osThreadAttr_t dcSampingTask_attributes = {
  .name = "dcSampingTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void shellTask(void *argument);
void StartChipTemperatureTask(void *argument);
void StartOledDisplayTask(void *argument);
void StartKeyTask(void *argument);
void StartDCSampingTask(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  ad7606_Init();
  MX_USB_DEVICE_Init();
  userShellInit();
  three_Phase_Init_V(&signal_V, 50, 20000);
  three_Phase_Init_I(&signal_I, 50, 20000);
  ad7606_Start(&htim2, TIM_CHANNEL_1);
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of letterShellTask */
  letterShellTaskHandle = osThreadNew(shellTask, (void*) &shell, &letterShellTask_attributes);

  /* creation of chipTemperature */
  chipTemperatureHandle = osThreadNew(StartChipTemperatureTask, NULL, &chipTemperature_attributes);

  /* creation of oledDisplayTask */
  oledDisplayTaskHandle = osThreadNew(StartOledDisplayTask, NULL, &oledDisplayTask_attributes);

  /* creation of keyTask */
  keyTaskHandle = osThreadNew(StartKeyTask, NULL, &keyTask_attributes);

  /* creation of dcSampingTask */
  dcSampingTaskHandle = osThreadNew(StartDCSampingTask, NULL, &dcSampingTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
__weak void StartDefaultTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
  UNUSED(argument);
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_shellTask */
/**
 * @brief Function implementing the letterShellTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_shellTask */
__weak void shellTask(void *argument)
{
  /* USER CODE BEGIN shellTask */
  UNUSED(argument);
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END shellTask */
}

/* USER CODE BEGIN Header_StartChipTemperatureTask */
/**
 * @brief Function implementing the chipTemperature thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartChipTemperatureTask */
__weak void StartChipTemperatureTask(void *argument)
{
  /* USER CODE BEGIN StartChipTemperatureTask */
  UNUSED(argument);
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartChipTemperatureTask */
}

/* USER CODE BEGIN Header_StartOledDisplayTask */
/**
 * @brief Function implementing the oledDisplayTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartOledDisplayTask */
__weak void StartOledDisplayTask(void *argument)
{
  /* USER CODE BEGIN StartOledDisplayTask */
  UNUSED(argument);
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartOledDisplayTask */
}

/* USER CODE BEGIN Header_StartKeyTask */
/**
 * @brief Function implementing the keyTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartKeyTask */
__weak void StartKeyTask(void *argument)
{
  /* USER CODE BEGIN StartKeyTask */
  UNUSED(argument);
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartKeyTask */
}

/* USER CODE BEGIN Header_StartDCSampingTask */
/**
 * @brief Function implementing the dcSampingTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDCSampingTask */
__weak void StartDCSampingTask(void *argument)
{
  /* USER CODE BEGIN StartDCSampingTask */
  UNUSED(argument);
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDCSampingTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

