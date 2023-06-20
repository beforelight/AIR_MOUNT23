#pragma once
#include "fifo.h"

extern FIFO shell_fifo;
extern FIFO shell_tx_fifo;;

#ifdef __cplusplus
extern "C"{
#endif

#include "shell.h"
extern Shell lshell;

int myPrintf(const char *formatString, ...);

void shell_init();
void shell_tx_service();
void shell_rx_service();

static inline void shell_put(char ch)
{
  if (shell_fifo.buffer != NULL) {
    FIFO_Put(&shell_fifo, &ch, 1);
  }
}


#ifdef __cplusplus
};
#endif