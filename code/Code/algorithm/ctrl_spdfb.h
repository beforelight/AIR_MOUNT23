#ifndef CAPTURE_CONTROL_CTRL_SPD_FB_H
#define CAPTURE_CONTROL_CTRL_SPD_FB_H
#include "algo_ss.hpp"
#include "ctrl.hpp"
#include "ctrl_conf.h"
#include "ctrl_impl.h"
class ctrl_spdfb1 : public control::algorithm
{
public:
  algo_pid spdfb1_x{(float) CTRL_OVER_SAMPLE / (float) CTRL_FREQ, 2, -2, "spdfb1_x"};
  algo_pid spdfb1_y{(float) CTRL_OVER_SAMPLE / (float) CTRL_FREQ, 2, -2, "spdfb1_y"};
  algo_pid spdfb1_z{(float) CTRL_OVER_SAMPLE / (float) CTRL_FREQ, 1, -1, "spdfb1_z"};

public:
  void Init() override
  {
    spdfb1_x.kp = 0.1f;
    spdfb1_y.kp = 0.1f;
    spdfb1_z.kp = 0.01f;
    par_agent::get_instance().reg(spdfb1_x);
    par_agent::get_instance().reg(spdfb1_y);
    par_agent::get_instance().reg(spdfb1_z);
  }
  void Process() override
  {
    spdfb1_x.fb = ctrl.impl->sensor_941b_xyz[0];
    spdfb1_y.fb = ctrl.impl->sensor_941b_xyz[1];
    spdfb1_z.fb = ctrl.impl->sensor_941b_xyz[2];
    spdfb1_x.update();
    spdfb1_y.update();
    spdfb1_z.update();

    ctrl.impl->ta115_LAC08_xyz[0] = spdfb1_x.u;
    ctrl.impl->ta115_LAC08_xyz[1] = spdfb1_y.u;
    ctrl.impl->ta115_LAC08_xyz[2] = spdfb1_z.u;

    // 转换到 原始坐标
    ctrl.impl->ta115_LAC08 = ctrl.impl->Ta_mat * ctrl.impl->ta115_LAC08_xyz;
  }
};

class ctrl_spdfb2 : public control::algorithm
{
public:
  ss::siso<2> ss_x{
      2, -2, 1.0,                                                            //
      Eigen::Matrix<float, 2, 2>({{0.961334394040494, 0.000275426946833024}, //
                                  {0.0491713738958992, 0.976371035734198}}), //
      {0.000120419320156666, 0.0135714554904508},                            //
      {0.00907609114408684, -0.008012487664981},                             //
      {4.11456072768221, 2.22773111669996}};

  ss::siso<2> ss_y{
      2, -2, 1.0,                                                            //
      Eigen::Matrix<float, 2, 2>({{0.961334394040494, 0.000275426946833024}, //
                                  {0.0491713738958992, 0.976371035734198}}), //
      {0.000120419320156666, 0.0135714554904508},                            //
      {0.00907609114408684, -0.008012487664981},                             //
      {4.11456072768221, 2.22773111669996}};
  ss::siso<2> ss_z{
      0.4, -0.4, 1.0,                                                       //
      Eigen::Matrix<float, 2, 2>({{0.774225168537095, 0.0010333447310939},  //
                                  {0.193488870086748, 0.959408568160672}}), //
      {0.000403575474686402, 0.0452320072499727},                           //
      {0.00627059775600494, -0.00454389906467226},                          //
      {4.83893232228188, 4.62090182126727}};

public:
  void Init() override {}
  void Process() override
  {
    ctrl.impl->ta115_LAC08_xyz[0] = ss_x.update(0, ctrl.impl->sensor_941b_xyz[0]);
    ctrl.impl->ta115_LAC08_xyz[1] = ss_y.update(0, ctrl.impl->sensor_941b_xyz[1]);
    ctrl.impl->ta115_LAC08_xyz[2] = ss_z.update(0, (0.125f / 0.18f) * ctrl.impl->sensor_941b_xyz[2]);

    // 转换到 原始坐标
    ctrl.impl->ta115_LAC08 = ctrl.impl->Ta_mat * ctrl.impl->ta115_LAC08_xyz;
  }
};

class ctrl_spdfb2p1 : public ctrl_spdfb2
{
public:
  void Init() override
  {
    ss_x = ss_y = ss::siso<2>(
        2, -2, 1.0,                                                            //
        Eigen::Matrix<float, 2, 2>({{0.961334394040494, 0.000275426946833024}, //
                                    {0.0491713738958992, 0.976371035734198}}), //
        {0.000120419320156666, 0.0135714554904508},                            //
        {0.00907609114408684, -0.008012487664981},                             //
        {5.61456072768221, 6.16523111669996});
    ss_z = ss::siso<2>(
        0.4, -0.4, 1.0,                                                       //
        Eigen::Matrix<float, 2, 2>({{0.774225168537095, 0.0010333447310939},  //
                                    {0.193488870086748, 0.959408568160672}}), //
        {0.000403575474686402, 0.0452320072499727},                           //
        {0.00627059775600494, -0.00454389906467226},                          //
        {5.49518232228188, 10.1169955712673});
  }
};

class ctrl_spdfb2p2 : public ctrl_spdfb2
{
public:
  void Init() override
  {
    ss_x = ss_y = ss::siso<2>(
        2, -2, 1.0,                                                            //
        Eigen::Matrix<float, 2, 2>({{0.961334394040494, 0.000275426946833024}, //
                                    {0.0491713738958992, 0.976371035734198}}), //
        {0.000120419320156666, 0.0135714554904508},                            //
        {0.00907609114408684, -0.008012487664981},                             //
        {8.86456072768221, 14.6964811167});
    ss_z = ss::siso<2>(
        0.4, -0.4, 1.0,                                                       //
        Eigen::Matrix<float, 2, 2>({{0.774225168537095, 0.0010333447310939},  //
                                    {0.193488870086748, 0.959408568160672}}), //
        {0.000403575474686402, 0.0452320072499727},                           //
        {0.00627059775600494, -0.00454389906467226},                          //
        {5.74518232228188, 12.2107455712673});
  }
};

class ctrl_spdfb2p3 : public ctrl_spdfb2
{
public:
  void Init() override
  {
    ss_x = ss_y = ss::siso<2>(
        2, -2, 1.0,                                                            //
        Eigen::Matrix<float, 2, 2>({{0.961334394040494, 0.000275426946833024}, //
                                    {0.0491713738958992, 0.976371035734198}}), //
        {0.000120419320156666, 0.0135714554904508},                            //
        {0.00907609114408684, -0.008012487664981},                             //
        {27.6145607276822, 63.9152311167});
    ss_z = ss::siso<2>(
        0.4, -0.4, 1.0,                                                       //
        Eigen::Matrix<float, 2, 2>({{0.774225168537095, 0.0010333447310939},  //
                                    {0.193488870086748, 0.959408568160672}}), //
        {0.000403575474686402, 0.0452320072499727},                           //
        {0.00627059775600494, -0.00454389906467226},                          //
        {10.4326823222819, 51.4685580712673});
  }
};
#endif // CAPTURE_CONTROL_CTRL_SPD_FB_H
