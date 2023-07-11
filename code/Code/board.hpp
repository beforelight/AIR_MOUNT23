#ifndef CAPTURE_CONTROL_BOARD_H
#define CAPTURE_CONTROL_BOARD_H
#include "drv_ad5360.h"
#include "drv_ad7606.h"
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "par_number.hpp"
#include <vector>
class board {
public:
    void Init();

public:
    std::vector<ad7606> adc = {
            ad7606(
                    IO(),
                    {ADC0_CVST_GPIO_Port, ADC0_CVST_Pin},
                    IO(SPI2_NCS_GPIO_Port,SPI2_NCS_Pin),
                    {ADC0_BUSY_GPIO_Port, ADC0_BUSY_Pin},
                    &hspi2),
            ad7606(
                    IO(),
                    {ADC0_CVST_GPIO_Port, ADC0_CVST_Pin},
                    IO(SPI3_NCS_GPIO_Port,SPI3_NCS_Pin),
                    {ADC0_BUSY_GPIO_Port, ADC0_BUSY_Pin},
                    &hspi3),
            ad7606(
                    IO(),
                    {ADC0_CVST_GPIO_Port, ADC0_CVST_Pin},
                    IO(SPI4_NCS_GPIO_Port,SPI4_NCS_Pin),
                    {ADC0_BUSY_GPIO_Port, ADC0_BUSY_Pin},
                    &hspi4)
    };


    ad5360 dac = {IO(), IO(), IO(), IO(), IO(), &hspi1};


    std::vector<IO> led = {
            {LD1_GPIO_Port, LD1_Pin},
            {LD2_GPIO_Port, LD2_Pin},
            {LD3_GPIO_Port, LD3_Pin}};

    std::vector<IO> MD = {
            {IO0_GPIO_Port,  IO0_Pin},
            {IO1_GPIO_Port,  IO1_Pin},
            {IO2_GPIO_Port,  IO2_Pin},
            {IO3_GPIO_Port,  IO3_Pin},
            {IO4_GPIO_Port,  IO4_Pin},
            {IO5_GPIO_Port,  IO5_Pin},
            {IO6_GPIO_Port,  IO6_Pin},
            {IO7_GPIO_Port,  IO7_Pin},
            {IO8_GPIO_Port,  IO8_Pin},
            {IO9_GPIO_Port,  IO9_Pin},
            {IO10_GPIO_Port, IO10_Pin},
            {IO11_GPIO_Port, IO11_Pin},
            {IO12_GPIO_Port, IO12_Pin},
            {IO13_GPIO_Port, IO13_Pin},
            {IO14_GPIO_Port, IO14_Pin},
            {IO15_GPIO_Port, IO15_Pin}
    };

    TIM_HandleTypeDef *htim_counter = &htim2; // 计时器，1tick=1us
    uint32_t htim_counter_data[12]{0};         // 计数器BUF

    volatile unsigned int flag_adc_busy_mask = 0; // adc采集完成就在相应位置reset
    //  volatile bool flag_server_tick = false;

    volatile unsigned int skip_frame = 0; // 跳帧计数

    UART_HandleTypeDef *dbg_uart = &huart3;
    TIM_HandleTypeDef *htim_pit = &htim13; // 2ms周期触发
    par_number::number <uint32_t> ctrl_freq{500, "ctrl_freq"};
};

extern board brd;
#endif // CAPTURE_CONTROL_BOARD_H
