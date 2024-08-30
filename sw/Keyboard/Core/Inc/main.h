/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l5xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ROW3_Pin GPIO_PIN_2
#define ROW3_GPIO_Port GPIOE
#define COLMUXC_Pin GPIO_PIN_3
#define COLMUXC_GPIO_Port GPIOE
#define ROW2_Pin GPIO_PIN_4
#define ROW2_GPIO_Port GPIOE
#define COLMUXA_Pin GPIO_PIN_5
#define COLMUXA_GPIO_Port GPIOE
#define COLMUXB_Pin GPIO_PIN_6
#define COLMUXB_GPIO_Port GPIOE
#define COLOUTA_Pin GPIO_PIN_5
#define COLOUTA_GPIO_Port GPIOF
#define ROW1_Pin GPIO_PIN_0
#define ROW1_GPIO_Port GPIOC
#define COLOUTB_Pin GPIO_PIN_1
#define COLOUTB_GPIO_Port GPIOG
#define ROW4_Pin GPIO_PIN_5
#define ROW4_GPIO_Port GPIOD
#define ROW0_Pin GPIO_PIN_6
#define ROW0_GPIO_Port GPIOD
#define LOGIC_TRANS_EN_Pin GPIO_PIN_6
#define LOGIC_TRANS_EN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
