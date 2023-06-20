#include "halCallBack.h"
#include "ctrl.hpp"
#include "drv_schedule.h"
#include "drv_shell.h"
#include "up_data_format.hpp"
#include "usart.h"
#include <cstdio>
#include "drv_shell.h"
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM13) // AD采集定时器   2khz (前一周期用来触发采集，后一个周期用来跳帧)
  {
    // 采集周期，这里只触发采集，其余在其他中断处理
    brd.adc0.m_convsta = 0;
    brd.adc1.m_convsta = 0;
    brd.adc0.m_convsta = 1;
    brd.adc1.m_convsta = 1;
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (brd.adc0.IsOpen && GPIO_Pin == brd.adc0.m_busy.m_GPIO_Pin) {
    // ADC0
    if ((brd.flag_adc_busy_mask & 0b01) != 0) {
      printf("ADC0跳帧\r\n");
      brd.adc0.StopRead();
    }
    brd.flag_adc_busy_mask |= 0b01;
    brd.led[0] = 0;
    brd.adc0.StartOfRead(
        &brd.adc0.m_osp_buf[brd.adc_osp_counter * 8]); // 转换完成之后读取
  } else if (brd.adc1.IsOpen && GPIO_Pin == brd.adc1.m_busy.m_GPIO_Pin) {
    // ADC1
    if ((brd.flag_adc_busy_mask & 0b10) != 0) {
      printf("ADC1跳帧\r\n");
      brd.adc1.StopRead();
    }
    brd.flag_adc_busy_mask |= 0b10;
    brd.led[1] = 0;
    brd.adc1.StartOfRead(
        &brd.adc1.m_osp_buf[brd.adc_osp_counter * 8]); // 转换完成之后读取
  }
}

void ctrl_process()
{
  __HAL_TIM_GET_COUNTER(brd.htim_counter) = 0;
  brd.adc0.EndOfOversample(updata_fifo.head().adc_buf[0]);
  brd.adc1.EndOfOversample(updata_fifo.head().adc_buf[1]);

  brd.dac0.SetBuf(
      updata_fifo.head().dac_buf[0]);
  ctrl.Process();
  updata_fifo.put_head();
}

void if_end_of_over_sample()
{
  brd.adc_osp_counter++;
  if (brd.adc_osp_counter >= CTRL_OVER_SAMPLE) {
    brd.adc_osp_counter = 0;
    // 唤起控制服务程序
    ScheduleActive(ctrl_process);
  }
}
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
  if (hspi == brd.adc0.m_spi) {
    brd.adc0.EndOfRead();              // 读取完成 关闭片选
    brd.flag_adc_busy_mask &= ~(0b01); // 相应位置清零
    brd.led[0] = 1;                    // 关灯
    if (brd.flag_adc_busy_mask == 0) {
      if_end_of_over_sample();
    }
  } else if (hspi == brd.adc1.m_spi) {
    brd.adc1.EndOfRead();              // 读取完成 关闭片选
    brd.flag_adc_busy_mask &= ~(0b10); // 相应位置清零
    brd.led[1] = 1;                    // 关灯
    if (brd.flag_adc_busy_mask == 0) {
      if_end_of_over_sample();
    }
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart==&huart2){
    shell_tx_service();
  }
}