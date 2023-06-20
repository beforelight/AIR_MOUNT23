#ifndef CAPTURE_CONTROL_CTRL_POS_HINF1_H
#define CAPTURE_CONTROL_CTRL_POS_HINF1_H
#include "ctrl.hpp"
#include "ctrl_impl.h"
#include "iir_df2tf.hpp"
class ctrl_pos_hinf1 : public control::algorithm
{
public:
  void Process() override
  {
    // 计算hinf
    ctrl.impl->ta115_LAC08_xyz[0] = hinf_x.update(ctrl.impl->sensor_cd22_xyz[0]);
    ctrl.impl->ta115_LAC08_xyz[1] = hinf_y.update(ctrl.impl->sensor_cd22_xyz[1]);
    ctrl.impl->ta115_LAC08_xyz[2] = 0;

    // 转换到 原始坐标
    ctrl.impl->ta115_LAC08 = ctrl.impl->Ta_mat * ctrl.impl->ta115_LAC08_xyz;
  }
  void Init() override {}

private:
  iir_df2tf<float, 3> hinf_x{
      {-577.7, 2.449, 580.2},
      {1, 1.892, 0.8975}};
  iir_df2tf<float, 3> hinf_y{
      {-577.7, 2.449, 580.2},
      {1, 1.892, 0.8975}};
};

class ctrl_pos_hinf2 : public control::algorithm
{
public:
  void Process() override
  {
    // 计算hinf
    ctrl.impl->ta115_LAC08_xyz[0] = hinf_x.update(ctrl.impl->sensor_cd22_xyz[0]);
    ctrl.impl->ta115_LAC08_xyz[1] = hinf_y.update(ctrl.impl->sensor_cd22_xyz[1]);
    ctrl.impl->ta115_LAC08_xyz[2] = 0;

    // 转换到 原始坐标
    ctrl.impl->ta115_LAC08 = ctrl.impl->Ta_mat * ctrl.impl->ta115_LAC08_xyz;
  }
  void Init() override {}

private:
  iir_df2tf<float, 5> hinf_x{
      {-0.00286, -0.005626, 0.0002809, 0.006, 0.002953},
      {1, -0.8482, -0.85, 0.3061, 0.3938}};
  iir_df2tf<float, 5> hinf_y{
      {-0.00286, -0.005626, 0.0002809, 0.006, 0.002953},
      {1, -0.8482, -0.85, 0.3061, 0.3938}};
};
#endif // CAPTURE_CONTROL_CTRL_POS_HINF1_H
