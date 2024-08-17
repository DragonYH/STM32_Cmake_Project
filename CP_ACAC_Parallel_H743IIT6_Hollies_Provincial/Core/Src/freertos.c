/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
/* Definitions for stateLED */
osThreadId_t stateLEDHandle;
const osThreadAttr_t stateLED_attributes = {
  .name = "stateLED",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for oledDisplay */
osThreadId_t oledDisplayHandle;
const osThreadAttr_t oledDisplay_attributes = {
  .name = "oledDisplay",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for mainControl */
osThreadId_t mainControlHandle;
const osThreadAttr_t mainControl_attributes = {
  .name = "mainControl",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for keyControl */
osThreadId_t keyControlHandle;
const osThreadAttr_t keyControl_attributes = {
  .name = "keyControl",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for stateControl */
osThreadId_t stateControlHandle;
const osThreadAttr_t stateControl_attributes = {
  .name = "stateControl",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartStateLED(void *argument);
void StartOledDisplay(void *argument);
void StartMainControl(void *argument);
void StartKeyControl(void *argument);
void StartStateControl(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  UserInit();
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
  /* creation of stateLED */
  stateLEDHandle = osThreadNew(StartStateLED, NULL, &stateLED_attributes);

  /* creation of oledDisplay */
  oledDisplayHandle = osThreadNew(StartOledDisplay, NULL, &oledDisplay_attributes);

  /* creation of mainControl */
  mainControlHandle = osThreadNew(StartMainControl, NULL, &mainControl_attributes);

  /* creation of keyControl */
  keyControlHandle = osThreadNew(StartKeyControl, NULL, &keyControl_attributes);

  /* creation of stateControl */
  stateControlHandle = osThreadNew(StartStateControl, NULL, &stateControl_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartStateLED */
/**
 * @brief  Function implementing the stateLED thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartStateLED */
__weak void StartStateLED(void *argument)
{
  /* USER CODE BEGIN StartStateLED */
  UNUSED(argument);
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartStateLED */
}

/* USER CODE BEGIN Header_StartOledDisplay */
/**
 * @brief Function implementing the oledDisplay thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartOledDisplay */
__weak void StartOledDisplay(void *argument)
{
  /* USER CODE BEGIN StartOledDisplay */
  UNUSED(argument);
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartOledDisplay */
}

/* USER CODE BEGIN Header_StartMainControl */
/**
 * @brief Function implementing the mainControl thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartMainControl */
__weak void StartMainControl(void *argument)
{
  /* USER CODE BEGIN StartMainControl */
  UNUSED(argument);
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartMainControl */
}

/* USER CODE BEGIN Header_StartKeyControl */
/**
 * @brief Function implementing the keyControl thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartKeyControl */
__weak void StartKeyControl(void *argument)
{
  /* USER CODE BEGIN StartKeyControl */
  UNUSED(argument);
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartKeyControl */
}

/* USER CODE BEGIN Header_StartStateControl */
/**
 * @brief Function implementing the stateControl thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartStateControl */
__weak void StartStateControl(void *argument)
{
  /* USER CODE BEGIN StartStateControl */
  UNUSED(argument);
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartStateControl */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

