#ifndef CAPTURE_CONTROL_CTRL_DO_NOTHING_H
#define CAPTURE_CONTROL_CTRL_DO_NOTHING_H
#include "ctrl.hpp"
class ctrl_do_nothing : public control::algorithm
{
public:
  void Process() override {}       // 控制程序
  void Init() override {}          // 初始化等
};

#endif // CAPTURE_CONTROL_CTRL_DO_NOTHING_H
