#include "avis.h"
#include "board.hpp"
#include "ctrl.hpp"
#include "drv_schedule.h"
#include "drv_shell.h"
#include "lfs_port.h"
#include "lwip.h"
#include "tcp_echoserver.h"
#include "udp_connect.h"
#include "udp_echoserver.h"
#include <cstdio>
#include "par_agent.hpp"
extern "C" [[noreturn]] void avis() {
    ScheduleInit(); // 先初始化任务调度
    shell_init();
    FLASH_Init(); // 这个要先于ctrl初始化，后于shell
    brd.Init();
    ctrl.Init();
    //   MX_LWIP_Init();
    //  udp_echoserver_init(); //测试UDP
    //  tcp_echoserver_init();  //测试TCP
    ConnectInit(); // 传输初始化
    par_agent::get_instance().load(); // 后于ctrl初始化
    //设置周期中断的时间
    {
        htim13.Init.Period = (1000000U / brd.ctrl_freq.val) - 1;
        if (HAL_TIM_Base_Init(&htim13) != HAL_OK) {
            Error_Handler();
        }
        HAL_TIM_Base_Start_IT(&htim13); // 打开周期中断的时间基准
    }
    printf("AVIS Start!\r\n");

    while (true) {
        shell_rx_service();
        shell_tx_service();
        MX_LWIP_Process();
        ConnectProcess();
    }
}