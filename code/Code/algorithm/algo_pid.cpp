//
// Created by 17616 on 2022/10/12.
//

#include "algo_pid.h"
void algo_pid::update()
{
  // 更新控制
  err0 = ref - fb;
  float err_diff01 = err0 - err1;
  float err_diff12 = err1 - err2;
  float delta_u = kp * err_diff01
                  + ki * Ts * err0
                  + kd * Ts_inver * (err_diff01 - err_diff12);
  u += delta_u;
  // 差分
  err2 = err1;
  err1 = err0;
  // 限幅
  if (u > u_max) {
    u = u_max;
  }
  if (u < u_min) {
    u = u_min;
  }
}
std::string algo_pid::str()
{
  std::string str;
  str += std::to_string(kp);
  str += ' ';
  str += std::to_string(ki);
  str += ' ';
  str += std::to_string(kd);
  str += ' ';
  return str;
}

void algo_pid::scan(const std::string &str)
{
  size_t space[3];
  space[0] = str.find(' ', 0);
  for (int i = 1; i < 3; ++i) {
    space[i] = space[i - 1] != std::string::npos ? str.find(' ', space[i - 1] + 1) : std::string::npos;
  }

  if (space[2] != std::string::npos && space[0] > 0 && space[1] - space[0] > 0 && space[2] - space[1] > 0) {
    kp = std::stof(str.substr(0, space[0]));
    ki = std::stof(str.substr(space[0], space[1] - space[0]));
    kd = std::stof(str.substr(space[1], space[2] - space[1]));
  }
}
