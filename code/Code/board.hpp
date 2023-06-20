#ifndef CAPTURE_CONTROL_BOARD_H
#define CAPTURE_CONTROL_BOARD_H
#include "ctrl_conf.h"
#include "drv_ad5360.h"
#include "drv_ad7606.h"
#include "main.h"
#include "spi.h"
#include "tim.h"
#include <vector>
extern int16_t board_adc0_osp_buf[CTRL_OVER_SAMPLE * 8];
extern int16_t board_adc1_osp_buf[CTRL_OVER_SAMPLE * 8];
class board
{
public:
  void Init();

public:
  ad7606 adc0 = ad7606(
      {SPI1_RESET_GPIO_Port, SPI1_RESET_Pin},
      {SPI1_CONA_GPIO_Port, SPI1_CONA_Pin},
      {SPI1_CS_GPIO_Port, SPI1_CS_Pin},
      {SPI1_BUSY_GPIO_Port, SPI1_BUSY_Pin},
      &hspi1,
      board_adc0_osp_buf,
      CTRL_OVER_SAMPLE);

  ad7606 adc1 = ad7606(
      {SPI6_RESET_GPIO_Port, SPI6_RESET_Pin},
      {SPI6_CONA_GPIO_Port, SPI6_CONA_Pin},
      {SPI6_CS_GPIO_Port, SPI6_CS_Pin},
      {SPI6_BUSY_GPIO_Port, SPI6_BUSY_Pin},
      &hspi6,
      board_adc1_osp_buf,
      CTRL_OVER_SAMPLE);

  ad5360 dac0 = ad5360(
      {DAC_RESET_GPIO_Port, DAC_RESET_Pin},
      {DAC_BUSY_GPIO_Port, DAC_BUSY_Pin},
      {DAC_CLR_GPIO_Port, DAC_CLR_Pin},
      {DAC_LDAC_GPIO_Port, DAC_LDAC_Pin},
      {SPI4_SYNC_GPIO_Port, SPI4_SYNC_Pin},
      &hspi4);

  std::vector<IO> led = {
      {LED0_GPIO_Port, LED0_Pin},
      {LED1_GPIO_Port, LED1_Pin},
      {LED2_GPIO_Port, LED2_Pin}};

  std::vector<IO> MD = {
      {MD0_GPIO_Port, MD0_Pin},
      {MD1_GPIO_Port, MD1_Pin},
      {MD2_GPIO_Port, MD2_Pin},
      {MD3_GPIO_Port, MD3_Pin},
      {MD4_GPIO_Port, MD4_Pin},
      {MD5_GPIO_Port, MD5_Pin},
  };

  TIM_HandleTypeDef *htim_counter = &htim14; // 计时器，1tick=1us
  uint16_t htim_counter_data[20]{0};         // 计数器BUF

  volatile unsigned int flag_adc_busy_mask = 0; // adc采集完成就在相应位置reset
                                                //  volatile bool flag_server_tick = false;

  volatile unsigned int skip_frame = 0; // 跳帧计数

  volatile unsigned int adc_osp_counter = 0; // adc过采样计数器
};

extern board brd;
#endif // CAPTURE_CONTROL_BOARD_H
