#ifndef CAPTURE_CONTROL_HALCALLBACK_H
#define CAPTURE_CONTROL_HALCALLBACK_H
#include "board.hpp"
#include "stm32f4xx_hal.h"
#ifdef __cplusplus
extern "C"{
#endif

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
#ifdef __cplusplus
};
#endif
#endif // CAPTURE_CONTROL_HALCALLBACK_H
