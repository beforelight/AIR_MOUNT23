#include "algo_pid.h"
#include "ctrl.hpp"
#include "ctrl_impl.hpp"

class ctrl_pos_pid : public control::algorithm {
public:
    algo_pid pid_pos_x;
    algo_pid pid_pos_y;
    algo_pid pid_pos_z;

public:
    void Process() override // 控制程序
    {
        // 计算PID
        pid_pos_x.fb = ctrl.impl->sensor_cd22_xyz[0];
        pid_pos_y.fb = ctrl.impl->sensor_cd22_xyz[1];
        pid_pos_z.fb = ctrl.impl->sensor_cd22_xyz[2];

        pid_pos_x.update();
        pid_pos_y.update();
        pid_pos_z.update();

        ctrl.impl->ta115_LAC08_xyz[0] = pid_pos_x.u;
        ctrl.impl->ta115_LAC08_xyz[1] = pid_pos_y.u;
        ctrl.impl->ta115_LAC08_xyz[2] = pid_pos_z.u;

        // 转换到 原始坐标
        ctrl.impl->ta115_LAC08 = ctrl.impl->Ta_mat * ctrl.impl->ta115_LAC08_xyz;
    }
    void Init() override // 初始化等
    {
        pid_pos_x = {1.0f/(float) brd.ctrl_freq.val, 2, -2, "pid_pos_xy"};
        pid_pos_y = {1.0f/(float) brd.ctrl_freq.val, 2, -2, "pid_pos_xy"};
        pid_pos_z = {1.0f / (float) brd.ctrl_freq.val, 1, -1, "pid_pos_z"};
        printf("当前控制周期为%f\r\n", 1.0f / (float) brd.ctrl_freq.val);
        //  pid_pos_y.kp = pid_pos_x.kp = 0.01 * 2798;
        //  pid_pos_y.ki = pid_pos_x.ki = 3775;
        pid_pos_y.kd = pid_pos_x.kd = 40000;

        //  pid_pos_z.kp = 0.01 * 77;
        //  pid_pos_z.ki = 149;
        pid_pos_z.kd = 4000;

        par_agent::get_instance().reg(pid_pos_x);
        par_agent::get_instance().reg(pid_pos_y);
        par_agent::get_instance().reg(pid_pos_z);
    }
    void EnterCallBack() override{
        pid_pos_x.clear();
        pid_pos_y.clear();
        pid_pos_z.clear();
    }
};
ctrl_pos_pid ctrlPosPid;
CTRL_EXPORT_ALGO(ctrlPosPid,"pos_pid","位置反馈PID控制");