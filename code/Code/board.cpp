#include "board.hpp"
#include "safe_lock.hpp"
#include "section.h"
int16_t board_adc0_osp_buf[CTRL_OVER_SAMPLE * 8];
int16_t board_adc1_osp_buf[CTRL_OVER_SAMPLE * 8];
BSS_CCMRAM board brd;
void board::Init()
{
  adc0.Init();
  adc1.Init();
  dac0.Init();
  HAL_TIM_Base_Start(htim_counter);
  for (auto i : htim_counter_data) {
    i = 0;
  }
  for (auto &i : MD) {
    i = 0;
  }

  for (auto &i : led) {
    i = 1;
  }
}

#include "drv_shell.h"
#include "usart.h"
extern "C" int _write(int fd, char *pBuffer, int size)
{
  if (shell_tx_fifo.buffer != NULL) {
    //    假如串口都准备好了就等，否则不等
    if (huart2.hdmatx != NULL && size < FIFO_Size(&shell_tx_fifo)) {
      while (
          size > FIFO_UnusedSize(&shell_tx_fifo) && FIFO_UnusedSize(&shell_tx_fifo) > 0) {
      } // 等
      safe_lock lock;
      FIFO_Put(&shell_tx_fifo, pBuffer, size);
    } else {
      safe_lock lock;
      FIFO_Put(&shell_tx_fifo, pBuffer, size);
    }
  }
  return size;
}
