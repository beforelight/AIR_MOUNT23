#ifndef AVIS_RT_CTRL_HPP
#define AVIS_RT_CTRL_HPP
#include <cstring>
/*

        //请确保链接脚本有以下内容，否则将找不到注册的控制算法
        _ctrl_pair_start = .;
        KEEP (*(ctrlPair))
        _ctrl_pair_end = .;

 */



#ifndef SHELL_SECTION
#if defined(__CC_ARM) || defined(__CLANG_ARM)
#define SHELL_SECTION(x)                __attribute__((section(x)))
#elif defined (__IAR_SYSTEMS_ICC__)
#define SHELL_SECTION(x)                @ x
#elif defined(__GNUC__)
#define SHELL_SECTION(x)                __attribute__((section(x)))
#else
#define SHELL_SECTION(x)
#endif
#endif

#ifndef SHELL_USED
#if defined(__CC_ARM) || defined(__CLANG_ARM)
#define SHELL_USED                      __attribute__((used))
#elif defined (__IAR_SYSTEMS_ICC__)
#define SHELL_USED                      __root
#elif defined(__GNUC__)
#define SHELL_USED                      __attribute__((used))
#else
#define SHELL_USED
#endif
#endif
#define CTRL_EXPORT_ALGO(_instance, _name, _desc) \
            extern "C" SHELL_USED SHELL_SECTION("ctrlPair") const control::pair ctrlPair_##_instance(_name,_instance,_desc);
class control_impl;
class control {
public:
    class algorithm // 如果有需要实现的控制算法,先继承这个虚基类
    {
    public:
        virtual ~algorithm() = default;
        virtual void Process() = 0;  // 控制程序
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
        pair(const char *intanceStr_, algorithm &intance_, const char *helpInfo_)
                : intanceStr(intanceStr_), intance(&intance_), helpInfo(helpInfo_) {}
        static const pair *begin();//返回pair对象的迭代器，pair对象由CTRL_EXPORT_ALGO注册到固件中
        static const pair *end();//返回pair对象的迭代器，pair对象由CTRL_EXPORT_ALGO注册到固件中
        static const pair *find(algorithm *instance);
        static const pair *find(const char *intanceStr);
    };

public:
    void Init();
    void Process() const;
    bool AlgorithmSwitch(const char *algo_str);
    explicit control(control_impl *impl_);
    ~control();

// for cmd.cpp
public:
    void sensor_zero(); // 确保位移传感器已经得到了1000个数据
    void sensor_zero_for_941b(); // 确保位移传感器已经得到了1000个数据
    void motor_zero();  // 确保位移传感器归零后，已输出了1000次控制
    void after_par_load(); //加载参数后调用，加快滤波器收敛
    void set_motor_chnnl_output(int chnnl, float val);
    void set_motor_xyz_output(int chnnl, float val);
public:
    control_impl *impl;
    algorithm *m_algorithmCurrent = nullptr;
};

extern control ctrl;

#endif //AVIS_RT_CTRL_HPP
