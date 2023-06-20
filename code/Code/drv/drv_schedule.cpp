#include "drv_schedule.h"
#include "safe_fifo.hpp"
#include "stm32f4xx_hal.h"

safe_fifo_with_buff<void (*)(void), 16> schedule_fifo;
bool ScheduleActive(void (*call)()) {
    {
        safe_lock lock;
        schedule_fifo.head() = call;
        schedule_fifo.put_head();
    }
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk; // 激活
    return true;
}
void ScheduleInit(unsigned int Priority) {
    HAL_NVIC_SetPriority(PendSV_IRQn, Priority, 0U);
    HAL_NVIC_EnableIRQ(PendSV_IRQn);
}
void ScheduleIRQHandler() {
    while (!schedule_fifo.empty()) {
        void (*call)();
        {
            safe_lock lock;
            call = schedule_fifo.tail();
            schedule_fifo.pop_tail();
        }
        call();
    }
    SCB->ICSR = SCB_ICSR_PENDSVCLR_Msk; // 清除flag
}
