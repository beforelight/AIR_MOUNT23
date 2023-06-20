#ifndef CAPTURE_CONTROL_ALGO_PID_H
#define CAPTURE_CONTROL_ALGO_PID_H
#include "par_agent.hpp"
class algo_pid : public par_agent::par
{
public:
  float ref = 0; // 目标
  float fb = 0;  // 反馈
  float u = 0;   // 输出
  float kp = 0;
  float ki = 0;
  float kd = 0;

private:
  // 运行中间变量
  float err0 = 0;
  float err1 = 0;
  float err2 = 0;

  float Ts = 0;
  float Ts_inver = 0;
  float u_max = 0;
  float u_min = 0;

public:
  /// \brief 构造
  /// \param T 控制周期，单位s
  /// \param u_max_ 输出限幅
  /// \param u_min_ 输出限幅
  /// \param id   参数的id,用于动态查找
  algo_pid(float T, float u_max_, float u_min_, const char *id)
      : Ts(T), Ts_inver(T), u_max(u_max_), u_min(u_min_),
        par_agent::par("pid", id) {}

  /// \brief 更新一帧控制，相应的变量为 ref,fb,u
  void update();

  // 内部参数怎么转换到字符串
  std::string str() override;
  // 内部参数怎么从字符串读取
  void scan(const std::string &str) override;
};

#endif // CAPTURE_CONTROL_ALGO_PID_H
