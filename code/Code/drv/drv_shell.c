#include "drv_shell.h"
#include "main.h"
#include "usart.h"
#include <stdarg.h>
#include <stdio.h>

Shell lshell;
char shellBuffer[2048];
FIFO shell_fifo;
FIFO shell_tx_fifo;

/// \brief 重定向printf
/// \param fd
/// \param pBuffer
/// \param size
/// \return
extern int _write(int fd, char *pBuffer, int size);

signed short userShellWrite(char *data, unsigned short len)
{
  _write(0, data, len);
  return (signed short) len;
}

void shell_init()
{
  FIFO_Init(&shell_fifo, 64);
  FIFO_Init(&shell_tx_fifo, 2048);
  lshell.write = userShellWrite;
  shellInit(&lshell, shellBuffer, sizeof(shellBuffer));
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
}

int myPrintf(const char *formatString, ...)
{
  char printBuf[256];
  va_list arg;
  int logLength;
  va_start(arg, formatString);
  logLength = vsnprintf(printBuf, sizeof(printBuf), formatString, arg);
  va_end(arg);
  //  shellWriteEndLine(&lshell, printBuf, logLength);
  _write(0, printBuf, logLength);
  return logLength;
}
void shell_tx_service()
{
  static char buf[64];
  if (shell_tx_fifo.buffer != NULL) {
    if (HAL_UART_STATE_READY==huart2.gState
        &&__HAL_DMA_GET_COUNTER(huart2.hdmatx) == 0
        && huart2.hdmatx->State == HAL_DMA_STATE_READY) {
      uint32_t size = MIN(FIFO_UsedSize(&shell_tx_fifo), sizeof(buf));
      if (size > 0) {
        uint32_t priMask = __get_PRIMASK();
        __disable_irq(); // 锁上
        size = MIN(FIFO_UsedSize(&shell_tx_fifo), sizeof(buf));
        // 如果缓存有数据就继续发
        if (size > 0) {
          FIFO_Get(&shell_tx_fifo, buf, size);
          if (HAL_BUSY == HAL_UART_Transmit_DMA(&huart2, (const uint8_t *) buf, size)) {
            __NOP();
          }
        }
        __set_PRIMASK(priMask); // 解锁
      }
    }
  }
}
void shell_rx_service()
{
  if (FIFO_UsedSize(&shell_fifo)) {
    char ch;
    FIFO_Get(&shell_fifo, &ch, 1);
    shellHandler(&lshell, ch);
  }
}
