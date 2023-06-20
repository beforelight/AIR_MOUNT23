#ifndef CAPTURE_CONTROL_CTRL_IMPL_H
#define CAPTURE_CONTROL_CTRL_IMPL_H
#include "Eigen"
// #include "ultraLowPassFilter.h"
#include "board.hpp"
#include "up_data_format.hpp"

using namespace Eigen;
#define SENSOR_941B_MODE (2U) // 定义941b是哪个档位
// 注意 此类用来代理ctrl类的所有成员变量，
// 主要目的 是 避免控制类的代码污染其他模块的代码
class control_impl
{
public:
  // 注意，由本class来完成ADC\DAC和物理量的转换
  // 算法则仅关注算法

  constexpr static const float axy = 0.12;   // axy是电机的
  constexpr static const float sxy = 0.18;   // sxy是位移传感器的
  constexpr static const float svxy = 0.125; // svxy是速度传感器的
  Matrix<float, 4, 3> Ta_mat{
      {0.5, 0, 0.25 / axy},
      {0.0, 0.5, 0.25 / axy},
      {-0.5, 0.0, 0.25 / axy},
      {0.0, -0.5, 0.25 / axy}};
  Matrix<float, 3, 4> Tss_mat{
      {-0.5, 0, 0.5, 0},
      {0, -0.5, 0, 0.5},
      {-0.25 / sxy, -0.25 / sxy, -0.25 / sxy, -0.25 / sxy}};
  Matrix<float, 3, 4> Tsv_mat{
      {0, 0.5, 0, -0.5},
      {-0.5, 0, 0.5, 0},
      {-0.25 / svxy, -0.25 / svxy, -0.25 / svxy, -0.25 / svxy}};
  constexpr static const float KF = 1.38;                                       // 每A多少N
  constexpr static const float adc_to_s = (10.0 / 32768.0);                     // 传感器每1mm 1V ADC:+-10V量程
  constexpr static const float f_to_dac = (32768.0 / 10.0 / KF / (8.0 / 10.0)); // DAC:+-10V量程 TA115:10V 8A档位
#if (SENSOR_941B_MODE == 2U)
  constexpr static const float adc_to_941b[4] = {
      0.01 * 1000.0 * ((10.0 / 32768.0) / 21.2290),
      0.01 * 1000.0 * ((10.0 / 32768.0) / 21.0274),
      0.01 * 1000.0 * ((10.0 / 32768.0) / 21.0607),
      0.01 * 1000.0 * ((10.0 / 32768.0) / 20.3816)};
#endif
  //  ultraLowPassFilter<4, 3> sensor_cd22_ultra_lowpass[4];

  /*以下变量供所有控制的类使用，但只有部分类有写入*/
  Vector4f sensor_cd22{0, 0, 0, 0};                                   // 单位mm 类control::algorithm 可写
  Vector4f sensor_cd22_raw{0, 0, 0, 0};                               // 单位mm 类control 可写
  Vector4f sensor_cd22_bias{0, 0, 0, 0};                              // 单位mm 类control 可写
  Vector4f sensor_cd22_lowpass{0, 0, 0, 0};                           // 单位mm 类control 可写
  Vector3f sensor_cd22_xyz{0, 0, 0};                                  // 单位mm 类control 可写
  Vector4f ta115_LAC08{0, 0, 0, 0};                                   // 单位N 类control::algorithm 可写
  Vector4f ta115_LAC08_output{0, 0, 0, 0};                            // 单位N 类control 可写
  Vector4f ta115_LAC08_lowpass{0, 0, 0, 0};                           // 单位N 类control 可写
  Vector4f ta115_LAC08_bias{-0.012693, 0.00939, 0.005044, -0.007029}; // 单位N 类control 可写
  Vector3f ta115_LAC08_xyz{0, 0, 0};                                  // 单位N 类control::algorithm 可写 //不能确定控制算法有没有投影到xyz上，所以由类control::algorithm决定

  Vector4f sensor_941b{0, 0, 0, 0};
  Vector4f sensor_941b_raw{0, 0, 0, 0};
  Vector3f sensor_941b_xyz{0, 0, 0};
  Vector4f sensor_941b_lowpass{0, 0, 0, 0}; // 单位N 类control 可写
  Vector4f sensor_941b_bias{0, 0, 0, 0};    // 单位N 类control 可写

  Vector4f sensor_941b_low{0, 0, 0, 0};
};
#endif // CAPTURE_CONTROL_CTRL_IMPL_H
