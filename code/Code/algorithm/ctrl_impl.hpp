#ifndef AVIS_RT_CTRL_IMPL_HPP
#define AVIS_RT_CTRL_IMPL_HPP
#include "Eigen/Eigen"
#include "board.hpp"
#include "up_data_format.hpp"
#include "sensor_941b.h"
using namespace Eigen;
#define SENSOR_941B_MODE_DEFAULT (1U) // 定义941b是哪个档位，注意 0代表1挡，1代表2挡，以此类推
#define SENSOR_941B_ADC_10V_MODE (false) //定义941b所在的adc使用的是不是+-10V的量程，否则就是5V
// 注意 此类用来代理ctrl类的所有成员变量，
// 主要目的 是 避免控制类的代码污染其他模块的代码
class control_impl {
public:
    // 注意，由本class来完成ADC\DAC和物理量的转换
    // 算法则仅关注算法


    constexpr static float axy = 0.12;   // axy是电机的
    constexpr static float sxy = 0.18;   // sxy是位移传感器的
    constexpr static float svxy = 0.125; // svxy是速度传感器的
    constexpr static float svxy_ground = 0.077; // svxy是地面速度传感器的
    Matrix<float, 4, 3> Ta_mat{
            {-0.5, 0,    -0.25 / axy},
            {0.0,  -0.5, -0.25 / axy},
            {0.5,  0.0,  -0.25 / axy},
            {0.0,  0.5,  -0.25 / axy}};
    Matrix<float, 3, 4> Tss_mat{
            {-0.5,        0,           0.5,         0},
            {0,           -0.5,        0,           0.5},
            {-0.25 / sxy, -0.25 / sxy, -0.25 / sxy, -0.25 / sxy}};
    Matrix<float, 3, 4> Tsv_mat{
            {0,            0.5,          0,            -0.5},
            {-0.5,         0,            0.5,          0},
            {-0.25 / svxy, -0.25 / svxy, -0.25 / svxy, -0.25 / svxy}};
    Matrix<float, 3, 4> Tsvg_mat{
            {0,                   0.5,                 0,                   -0.5},
            {-0.5,                0,                   0.5,                 0},
            {-0.25 / svxy_ground, -0.25 / svxy_ground, -0.25 / svxy_ground, -0.25 / svxy_ground}};
    constexpr static float KF = 1.38;                                       // 每A多少N
    constexpr static float adc_to_s = (10.0 / 32768.0);                     // 传感器每1mm 1V ADC:+-10V量程
//    constexpr static float f_to_dac = (32768.0 / (5.0 / 2.2) / KF / (8.0 / 10.0)); // DAC:+-(5/2.2)V量程 TA115:10V 8A档位 最大1.818A,2.5N
    constexpr static float f_to_dac = (32768.0 / (5.0 / 2.2) / KF / (2.0 / 10.0)); // DAC:+-(5/2.2)V量程 TA115:10V 8A档位 最大1.818A,2.5N
    constexpr static float dac_to_f = 1.0 / f_to_dac;
    float adc_to_941b[4];//这俩在Init中赋值了
    float adc_to_base_941b[4];//这俩在Init中赋值了
public:
    /*以下变量供所有控制的类使用，但只有部分类有写入*/
    Vector4f sensor_cd22{0, 0, 0, 0};                                   // 单位mm 类control::algorithm 可写
    Vector4f sensor_cd22_raw{0, 0, 0, 0};                               // 单位mm 类control 可写
    Vector4f sensor_cd22_bias{0, 0, 0, 0};                              // 单位mm 类control 可写
    Vector4f sensor_cd22_lowpass{0, 0, 0, 0};                           // 单位mm 类control 可写
    Vector3f sensor_cd22_xyz{0, 0, 0};                                  // 单位mm 类control 可写
    Vector4f ta115_LAC08{0, 0, 0, 0};                                   // 单位N 类control::algorithm 可写
    Vector4f ta115_LAC08_output{0, 0, 0, 0};                            // 单位N 类control 可写
    Vector4f ta115_LAC08_lowpass{0, 0, 0, 0};                           // 单位N 类control 可写
    Vector4f ta115_LAC08_bias{0, 0, 0, 0}; // 单位N 类control 可写
    Vector3f ta115_LAC08_xyz{0, 0, 0};                                  // 单位N 类control::algorithm 可写 //不能确定控制算法有没有投影到xyz上，所以由类control::algorithm决定

    Vector4f sensor_941b{0, 0, 0, 0};
    Vector4f sensor_941b_raw{0, 0, 0, 0};
    Vector3f sensor_941b_xyz{0, 0, 0};
    Vector4f sensor_941b_lowpass{0, 0, 0, 0};
    Vector4f sensor_941b_bias{0, 0, 0, 0};

    Vector4f base_sensor_941b{0, 0, 0, 0};
    Vector4f base_sensor_941b_raw{0, 0, 0, 0};
    Vector3f base_sensor_941b_xyz{0, 0, 0};
    Vector4f base_sensor_941b_lowpass{0, 0, 0, 0};
    Vector4f base_sensor_941b_bias{0, 0, 0, 0};
    void Init(); //注册相关变量
    void Convst(); // 转换ADC
    void Output(); // DAC 输出
};
#endif //AVIS_RT_CTRL_IMPL_HPP
