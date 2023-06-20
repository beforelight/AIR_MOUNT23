#include "halCallBack.h"
#include "ctrl.hpp"
#include "drv_schedule.h"
#include "drv_shell.h"
#include "up_data_format.hpp"
#include "usart.h"
#include <cstdio>
#include "drv_shell.h"
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == brd.htim_pit) // AD采集定时器
    {
        brd.htim_counter_data[0] = __HAL_TIM_GET_COUNTER(brd.htim_counter);//开始采样

        // 采集周期，这里只触发采集，其余在其他中断处理
        for (auto &i: brd.adc) {
            i.m_convsta = 0;
        }
        for (auto &i: brd.adc) {
            i.m_convsta = 1;
        }
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    for (int i = 0; i < brd.adc.size(); ++i) {
        auto &adc = brd.adc[i];
        if (adc.IsOpen && GPIO_Pin == adc.m_busy.m_GPIO_Pin) {
            if ((brd.flag_adc_busy_mask & ((1U << i))) != 0) {
                printf("%d跳\n", i);
                adc.StopRead();
            }
            brd.flag_adc_busy_mask |= 1U << i;
            brd.led[i] = 1;
            adc.StartOfRead();

            brd.htim_counter_data[1] = __HAL_TIM_GET_COUNTER(brd.htim_counter);//开始传输
            break;
        }
    }
}
//此函数已经是在PendCV中调用的了
void ctrl_process() {
    brd.htim_counter_data[2] = __HAL_TIM_GET_COUNTER(brd.htim_counter);//传输完成 开始控制
    //准备下一次的缓存
    updata_fifo.m_in = (++updata_fifo.m_in) & updata_fifo.m_size_mask;
    for (int i = 0; i < brd.adc.size(); ++i) {
        brd.adc[i].PrepareXferBuff(updata_fifo.m_buffer[updata_fifo.m_in].adc_buf[i]);
        brd.adc[i].SetUsingData(updata_fifo.m_buffer[updata_fifo.m_using].adc_buf[i]);
    }
    brd.dac.SetBuf(updata_fifo.m_buffer[updata_fifo.m_using].dac_buf[0]);
    ctrl.Process();
    updata_fifo.m_using = (++updata_fifo.m_using) & updata_fifo.m_size_mask;
    brd.htim_counter_data[7] = __HAL_TIM_GET_COUNTER(brd.htim_counter);//传输完成 开始控制
}
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
    for (int i = 0; i < brd.adc.size(); ++i) {
        auto &adc = brd.adc[i];
        if (hspi == adc.m_spi) {
            adc.EndOfRead();              // 读取完成 关闭片选
            brd.flag_adc_busy_mask &= ~(0b01); // 相应位置清零
            brd.led[i] = 0;                    // 关灯
            if (brd.flag_adc_busy_mask == 0) {
                ScheduleActive(ctrl_process);// 唤起控制服务程序
            };
            break;
        }
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &huart2) {
        shell_tx_service();
    }
}