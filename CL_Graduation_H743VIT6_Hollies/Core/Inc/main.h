/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OLED_CS_Pin GPIO_PIN_6
#define OLED_CS_GPIO_Port GPIOA
#define DIS_Pin GPIO_PIN_7
#define DIS_GPIO_Port GPIOE
#define AD7606_RST_Pin GPIO_PIN_14
#define AD7606_RST_GPIO_Port GPIOD
#define AD7606_CS_Pin GPIO_PIN_15
#define AD7606_CS_GPIO_Port GPIOD
#define AD7606_BUSY_Pin GPIO_PIN_8
#define AD7606_BUSY_GPIO_Port GPIOC
#define AD7606_BUSY_EXTI_IRQn EXTI9_5_IRQn
#define KEY3_Pin GPIO_PIN_10
#define KEY3_GPIO_Port GPIOA
#define KEY2_Pin GPIO_PIN_10
#define KEY2_GPIO_Port GPIOC
#define KEY1_Pin GPIO_PIN_11
#define KEY1_GPIO_Port GPIOC
#define KEY0_Pin GPIO_PIN_12
#define KEY0_GPIO_Port GPIOC
#define OLED_DC_Pin GPIO_PIN_5
#define OLED_DC_GPIO_Port GPIOD
#define OLED_RST_Pin GPIO_PIN_6
#define OLED_RST_GPIO_Port GPIOD
#define OLED_SDIN_Pin GPIO_PIN_7
#define OLED_SDIN_GPIO_Port GPIOD
#define OLED_SCLK_Pin GPIO_PIN_3
#define OLED_SCLK_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_5
#define LED1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
