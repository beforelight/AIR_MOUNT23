#ifndef CAPTURE_CONTROL_CTRL_POS_PID_INDIVID_H
#define CAPTURE_CONTROL_CTRL_POS_PID_INDIVID_H
#include "algo_pid.h"
#include "ctrl.hpp"
#include "ctrl_conf.h"
#include "ctrl_impl.h"
class ctrl_pos_pid_individ : public control::algorithm
{
public:
  algo_pid pid_pos_individ[4] = {
      {(float) CTRL_OVER_SAMPLE / (float) CTRL_FREQ, 1, -1, "pid_pos_individ"},
      {(float) CTRL_OVER_SAMPLE / (float) CTRL_FREQ, 1, -1, "pid_pos_individ"},
      {(float) CTRL_OVER_SAMPLE / (float) CTRL_FREQ, 1, -1, "pid_pos_individ"},
      {(float) CTRL_OVER_SAMPLE / (float) CTRL_FREQ, 1, -1, "pid_pos_individ"}};

  void Process() override // 控制程序
  {
    for (int i = 0; i < 4; ++i) {
      pid_pos_individ[i].fb = -ctrl.impl->sensor_cd22[i];
      pid_pos_individ[i].update();
      ctrl.impl->ta115_LAC08[i] = pid_pos_individ[i].u;
    }
  }
  void Init() override // 初始化等
  {
    for (auto &i : pid_pos_individ) {
      i.kd = 20000.0f; // 仅微分项，为阻尼
      par_agent::get_instance().reg(i);
    }
  }
};

#endif // CAPTURE_CONTROL_CTRL_POS_PID_INDIVID_H
