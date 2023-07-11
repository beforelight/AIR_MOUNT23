/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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
#define ADC2_CVST_Pin GPIO_PIN_3
#define ADC2_CVST_GPIO_Port GPIOE
#define SPI4_NCS_Pin GPIO_PIN_4
#define SPI4_NCS_GPIO_Port GPIOE
#define ADC2_BUSY_Pin GPIO_PIN_6
#define ADC2_BUSY_GPIO_Port GPIOE
#define ADC2_BUSY_EXTI_IRQn EXTI9_5_IRQn
#define USER_Btn_Pin GPIO_PIN_13
#define USER_Btn_GPIO_Port GPIOC
#define USER_Btn_EXTI_IRQn EXTI15_10_IRQn
#define IO0_Pin GPIO_PIN_0
#define IO0_GPIO_Port GPIOF
#define IO1_Pin GPIO_PIN_1
#define IO1_GPIO_Port GPIOF
#define IO2_Pin GPIO_PIN_2
#define IO2_GPIO_Port GPIOF
#define IO3_Pin GPIO_PIN_3
#define IO3_GPIO_Port GPIOF
#define IO4_Pin GPIO_PIN_4
#define IO4_GPIO_Port GPIOF
#define IO5_Pin GPIO_PIN_5
#define IO5_GPIO_Port GPIOF
#define IO6_Pin GPIO_PIN_10
#define IO6_GPIO_Port GPIOF
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOH
#define IO7_Pin GPIO_PIN_0
#define IO7_GPIO_Port GPIOC
#define RMII_MDC_Pin GPIO_PIN_1
#define RMII_MDC_GPIO_Port GPIOC
#define RMII_REF_CLK_Pin GPIO_PIN_1
#define RMII_REF_CLK_GPIO_Port GPIOA
#define RMII_MDIO_Pin GPIO_PIN_2
#define RMII_MDIO_GPIO_Port GPIOA
#define RMII_CRS_DV_Pin GPIO_PIN_7
#define RMII_CRS_DV_GPIO_Port GPIOA
#define RMII_RXD0_Pin GPIO_PIN_4
#define RMII_RXD0_GPIO_Port GPIOC
#define RMII_RXD1_Pin GPIO_PIN_5
#define RMII_RXD1_GPIO_Port GPIOC
#define LD1_Pin GPIO_PIN_0
#define LD1_GPIO_Port GPIOB
#define UART7_DE_Pin GPIO_PIN_9
#define UART7_DE_GPIO_Port GPIOE
#define ADC0_BUSY_Pin GPIO_PIN_15
#define ADC0_BUSY_GPIO_Port GPIOE
#define ADC0_BUSY_EXTI_IRQn EXTI15_10_IRQn
#define ADC0_CVST_Pin GPIO_PIN_11
#define ADC0_CVST_GPIO_Port GPIOB
#define SPI2_NCS_Pin GPIO_PIN_12
#define SPI2_NCS_GPIO_Port GPIOB
#define RMII_TXD1_Pin GPIO_PIN_13
#define RMII_TXD1_GPIO_Port GPIOB
#define LD3_Pin GPIO_PIN_14
#define LD3_GPIO_Port GPIOB
#define STLK_RX_Pin GPIO_PIN_8
#define STLK_RX_GPIO_Port GPIOD
#define STLK_TX_Pin GPIO_PIN_9
#define STLK_TX_GPIO_Port GPIOD
#define IO8_Pin GPIO_PIN_15
#define IO8_GPIO_Port GPIOD
#define IO9_Pin GPIO_PIN_2
#define IO9_GPIO_Port GPIOG
#define IO10_Pin GPIO_PIN_3
#define IO10_GPIO_Port GPIOG
#define IO11_Pin GPIO_PIN_4
#define IO11_GPIO_Port GPIOG
#define IO12_Pin GPIO_PIN_5
#define IO12_GPIO_Port GPIOG
#define IO13_Pin GPIO_PIN_6
#define IO13_GPIO_Port GPIOG
#define IO14_Pin GPIO_PIN_7
#define IO14_GPIO_Port GPIOG
#define IO15_Pin GPIO_PIN_8
#define IO15_GPIO_Port GPIOG
#define USART6_DE_Pin GPIO_PIN_8
#define USART6_DE_GPIO_Port GPIOC
#define USART1_DE_Pin GPIO_PIN_8
#define USART1_DE_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SPI3_NCS_Pin GPIO_PIN_15
#define SPI3_NCS_GPIO_Port GPIOA
#define UART4_DE_Pin GPIO_PIN_0
#define UART4_DE_GPIO_Port GPIOD
#define UART5_DE_Pin GPIO_PIN_1
#define UART5_DE_GPIO_Port GPIOD
#define USART2_DE_Pin GPIO_PIN_4
#define USART2_DE_GPIO_Port GPIOD
#define RMII_TX_EN_Pin GPIO_PIN_11
#define RMII_TX_EN_GPIO_Port GPIOG
#define RMII_TXD0_Pin GPIO_PIN_13
#define RMII_TXD0_GPIO_Port GPIOG
#define ADC1_CVST_Pin GPIO_PIN_15
#define ADC1_CVST_GPIO_Port GPIOG
#define ADC1_BUSY_Pin GPIO_PIN_3
#define ADC1_BUSY_GPIO_Port GPIOB
#define ADC1_BUSY_EXTI_IRQn EXTI3_IRQn
#define LD2_Pin GPIO_PIN_7
#define LD2_GPIO_Port GPIOB
#define UART8_DE_Pin GPIO_PIN_9
#define UART8_DE_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
