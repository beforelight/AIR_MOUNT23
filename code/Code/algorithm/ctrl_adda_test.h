#ifndef CAPTURE_CONTROL_CTRL_ADDA_TEST_H
#define CAPTURE_CONTROL_CTRL_ADDA_TEST_H
#include "board.hpp"
#include "ctrl.hpp"
#include "up_data_format.hpp"
class ctrl_adda_test : public control::algorithm
{
public:
  void Process() override // 控制程序
  {
    int tim_counter_idx = 0;
    brd.htim_counter_data[tim_counter_idx++] = __HAL_TIM_GET_COUNTER(brd.htim_counter);
    static float sin_table[17] = {
        0, 361.2417, 673.6956, 895.1633, 995.7342, 961.8256, 798.0172, 526.4322, 183.7495,
        -183.7495, -526.4322, -798.0172,
        -961.8256, -995.7342, -895.1633, -673.6956, -361.2417};
    static int idx = 0;

    for (int i = 0; i < 16; ++i) {
      brd.dac0[i] = sin_table[idx];
      UPLOAD_DATA_Ix_NOCHECK(i) = sin_table[idx];
      idx++;
      idx %= 17;
      brd.htim_counter_data[tim_counter_idx++] = __HAL_TIM_GET_COUNTER(brd.htim_counter);
    }
  }
  void Init() override {}          // 初始化等
};

#endif // CAPTURE_CONTROL_CTRL_ADDA_TEST_H
