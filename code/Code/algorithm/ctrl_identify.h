#ifndef CAPTURE_CONTROL_CTRL_POS_IDENTIFY_H
#define CAPTURE_CONTROL_CTRL_POS_IDENTIFY_H
#include "ctrl.hpp"
#include "ctrl_conf.h"
#include "ctrl_impl.h"
#include "rng.h"
#include "stm32f4xx_hal_rng.h"

float getRand() // vpp在-1到1之间的数
{
  uint32_t urnd = HAL_RNG_GetRandomNumber(&hrng);
  return (1.0 / (float) INT32_MAX) * (*((int32_t *) &urnd));
}

namespace ctrl_identify
{
class base : public control::algorithm
{
public:
  void Init() override {}
};

class on_x : public base
{
public:
  void Process() override
  {
    static int count = 0;
    count++;
    if (count % (40 * CTRL_FREQ / CTRL_OVER_SAMPLE) > (20 * CTRL_FREQ / CTRL_OVER_SAMPLE))
      ctrl.impl->ta115_LAC08_xyz[0] = 0.05;
    else
      ctrl.impl->ta115_LAC08_xyz[0] = -0.05;
    ctrl.impl->ta115_LAC08_xyz[1] = 0;
    ctrl.impl->ta115_LAC08_xyz[2] = 0;
    ctrl.impl->ta115_LAC08 = ctrl.impl->Ta_mat * ctrl.impl->ta115_LAC08_xyz;
  }
};

class on_y : public base
{
public:
  void Process() override
  {
    static int count = 0;
    count++;
    if (count % (40 * CTRL_FREQ / CTRL_OVER_SAMPLE) > (20 * CTRL_FREQ / CTRL_OVER_SAMPLE))
      ctrl.impl->ta115_LAC08_xyz[1] = 0.05;
    else
      ctrl.impl->ta115_LAC08_xyz[1] = -0.05;
    ctrl.impl->ta115_LAC08_xyz[0] = 0;
    ctrl.impl->ta115_LAC08_xyz[2] = 0;
    ctrl.impl->ta115_LAC08 = ctrl.impl->Ta_mat * ctrl.impl->ta115_LAC08_xyz;
  }
};

class on_z : public base
{
public:
  void Process() override
  {
    static int count = 0;
    count++;
    ctrl.impl->ta115_LAC08_xyz[0] = 0;
    ctrl.impl->ta115_LAC08_xyz[1] = 0;

    if (count % (40 * CTRL_FREQ / CTRL_OVER_SAMPLE) > (20 * CTRL_FREQ / CTRL_OVER_SAMPLE))
      ctrl.impl->ta115_LAC08_xyz[2] = 0.02;
    else
      ctrl.impl->ta115_LAC08_xyz[2] = -0.02;
    ctrl.impl->ta115_LAC08 = ctrl.impl->Ta_mat * ctrl.impl->ta115_LAC08_xyz;
  }
};

}; // namespace ctrl_identify

#endif // CAPTURE_CONTROL_CTRL_POS_IDENTIFY_H
