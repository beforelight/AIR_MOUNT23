#include "cmd.h"
#include "arg_parser.h"
#include "board.hpp"
#include "ctrl.hpp"
#include "lfs_port.h"
#include "par_agent.hpp"
#include "stm32f4xx_hal.h"
#include "algo_pid.h"
#include <cstdio>
extern "C" {

int board_rst(int argn, char **argv)
{
  printf("系统复位");
  __set_FAULTMASK(1);
  __NVIC_SystemReset();
  return 0;
}

int cmd_time_get(int argn, char **argv)
{
  arg::parser ps(argn, argv);
  arg::exit ext;
  ext.append("以上为执行到代码位置上的时间，单位us");

  for (int i = 0; i < sizeof(board::htim_counter_data) / sizeof(uint16_t); ++i) {
    if (brd.htim_counter_data[i] == 0)
      break;
    printf("%d\t%d\r\n", i, brd.htim_counter_data[i]);
  }
  return 0;
}
}
int algo_switch(int argn, char **argv)
{
  arg::parser ps(argn, argv);
  arg::exit ext;
  const char *help = "\r\n用法: %s <algo> 将运行的控制算法切换到algo";
  if (ps.size() <= 1) {
    ext.append(help, ps[0].c_str());
    return algo_list(argn, argv);
  } else {
    if (ctrl.AlgorithmSwitch(ps[1].c_str()))
      printf("已切换到%s\r\n", ps[1].c_str());
    else {
      printf("没有%s这个算法\r\n", ps[1].c_str());
      return algo_list(argn, argv);
    }
  }
  return -1;
}
int algo_list(int argn, char **argv)
{
  arg::parser ps(argn, argv);
  arg::exit ext;
  printf("支持的控制算法\r\n");
  auto algo = control::algorithmIntance;
  control::pair *currentPair = nullptr;
  ctrl.m_algorithmCurrent;
  while (algo->intanceStr != nullptr) {
    printf("%s\t%s\r\n", algo->intanceStr, algo->helpInfo);
    if (ctrl.m_algorithmCurrent == algo->intance)
      currentPair = algo;
    algo++;
  }
  if (currentPair != nullptr)
    printf("\r\n当前的控制算法为%s\r\n", currentPair->intanceStr);

  return 0;
}
int cmd_pid_list(int argn, char **argv)
{
  arg::parser ps(argn, argv);
  arg::exit ext;
  //  const char *help = "\r\n用法: %s <mode> 将运行状切换到mode";
  printf("pid\tkp\tki\tkd\r\n");
  auto &agent = par_agent::get_instance();
  for (auto &i : agent.findByType("pid")) {
    auto *pid = (algo_pid *) (i);
    printf("%s\t%f\t%f\t%f\r\n", pid->m_id.c_str(), pid->kp, pid->ki, pid->kd);
  }
  return 0;
}
int cmd_pid_set(int argn, char **argv)
{
  arg::parser ps(argn, argv);
  arg::exit ext;
  const char *help = "\r\n用法: %s <pid> <kp> <ki> <kd> 设置pid的三个参数";
  if (ps.size() == 5) {
    auto &agent = par_agent::get_instance();
    bool no_obj = true;
    auto list = agent.find(ps[1], "pid");
    for (auto &i : list) {
      auto *pid = (algo_pid *) i;
      printf("已设置:\t");
      pid->kp = std::stof(ps[2]);
      pid->ki = std::stof(ps[3]);
      pid->kd = std::stof(ps[4]);
      printf("%s\t%f\t%f\t%f\r\n", pid->m_id.c_str(), pid->kp, pid->ki, pid->kd);
      no_obj = false;
    }
    if (no_obj)
      ext.append("当前不存在%s的pid对象\r\n", ps[1].c_str());
  } else {
    ext.append(help, ps[0].c_str());
  }
  return 0;
}
int par_load(int argn, char **argv)
{
  arg::parser ps(argn, argv);
  arg::exit ext;
  par_agent::get_instance().load();
  return 0;
}
int par_save(int argn, char **argv)
{
  arg::parser ps(argn, argv);
  arg::exit ext;
  par_agent::get_instance().save();
  return 0;
}
int file_del(int argn, char **argv)
{
  arg::parser ps(argn, argv);
  arg::exit ext;
  lfs_file_t lff;
  if (0 == lfs_file_open(hlfs, &lff, "default", LFS_O_RDWR | LFS_O_CREAT | LFS_O_TRUNC)) {
    lfs_file_close(hlfs, &lff);
  }
  return 0;
}
int file_cat(int argn, char **argv)
{
  arg::parser ps(argn, argv);
  arg::exit ext;
  lfs_file_t lff;
  if (0 == lfs_file_open(hlfs, &lff, "default", LFS_O_RDWR)) {
    printf("文件大小为%d\r\n", (int) lfs_file_size(hlfs, &lff));
    char buf;
    while (lfs_file_read(hlfs, &lff, &buf, 1) > 0) {
      printf("%c", buf);
    }
    lfs_file_close(hlfs, &lff);
  } else {
    printf("文件为空\r\n");
  }
  return 0;
}

int motor_on(int argn, char **argv)
{
  for (auto &i : brd.MD) {
    i = 1;
  }
  return 0;
}
int motor_off(int argn, char **argv)
{
  for (auto &i : brd.MD) {
    i = 0;
  }
  return 0;
}
int cmd_dac(int argn, char **argv)
{
  arg::parser ps(argn, argv);
  arg::exit ext;
  const char *help = "\r\n用法: %s <channel> <val> 设置dac某通道的输出";
  if (ps.size() == 3) {
    int chnnl = std::stoi(ps[1]);
    int val = std::stoi(ps[2]);
    chnnl = std::max(chnnl, 0);
    chnnl = std::min(chnnl, 15);
    val = std::max(val, -32767);
    val = std::min(val, 32767);
    brd.dac0.Convert(chnnl, val);
    printf("dac[%d] = %d\r\n", chnnl, val);
  } else {
    ext.append(help, ps[0].c_str());
  }
  return 0;
}
int motor_output(int argn, char **argv)
{
  arg::parser ps(argn, argv);
  arg::exit ext;
  const char *help = "\r\n用法: %s <channel> <val(N)> 设置电机的出力，单位N";
  if (ps.size() == 3) {
    int chnnl = std::stoi(ps[1]);
    float val = std::stof(ps[2]);
    chnnl = std::max(chnnl, 0);
    chnnl = std::min(chnnl, 3);
    val = std::max(val, -1.0f);
    val = std::min(val, 1.0f);
    ctrl.set_motor_chnnl_output(chnnl, val);
    printf("ctrl.ta115_LAC08[%d] = %f\r\n", chnnl, val);
  } else {
    ext.append(help, ps[0].c_str());
  }
  return 0;
}
int sensor_zero(int argn, char **argv)
{
  arg::parser ps(argn, argv);
  arg::exit ext;
  ctrl.sensor_zero();
  return 0;
}
int motor_zero(int argn, char **argv)
{
  arg::parser ps(argn, argv);
  arg::exit ext;
  ctrl.motor_zero();
  return 0;
}
