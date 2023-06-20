#ifndef CAPTURE_CONTROL_CTRL_H
#define CAPTURE_CONTROL_CTRL_H
#include <cstring>
class control_impl;
class control
{
public:
  class algorithm // 如果有需要实现的控制算法,先继承这个虚基类
  {
  public:
    virtual ~algorithm() = default;
    virtual void Process() = 0;  // 控制程序 周期为CTRL_FREQ/CTRL_OVER_SAMPLE
    virtual void Init() = 0;     // 初始化等
    virtual void EnterCallBack();
    virtual void LeaveCallBack();
  };
  struct pair {
    const char *intanceStr;
    algorithm *intance;
    const char *helpInfo;
    pair(const char *intanceStr_, algorithm *intance_, const char *helpInfo_)
        : intanceStr(intanceStr_), intance(intance_), helpInfo(helpInfo_) {}
  };
  static pair algorithmIntance[]; // 在这个数组里面添加自定义的算法

public:
  void Convst() const; // 转换ADC
  void Output() const; // DAC 输出
  void Init();
  void Process() const;
  bool AlgorithmSwitch(const char *algo_str);
  static algorithm *FindAlgorithm(const char *algo_str);
  explicit control(control_impl *impl_);
  ~control();

  // for cmd.cpp
public:
  void sensor_zero(); // 确保位移传感器已经得到了1000个数据
  void motor_zero();  // 确保位移传感器归零后，已输出了1000次控制
  void set_motor_chnnl_output(int chnnl, float val);

public:
  control_impl *impl;
  algorithm *m_algorithmCurrent = nullptr;
};
extern control ctrl;
#endif // CAPTURE_CONTROL_CTRL_H
