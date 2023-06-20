#include "ctrl.hpp"
#include "ctrl_adda_test.h"
#include "ctrl_do_nothing.h"
#include "ctrl_identify.h"
#include "ctrl_impl.h"
#include "ctrl_pos_hinf.h"
#include "ctrl_pos_pid.h"
#include "ctrl_pos_pid_individ.h"
#include "ctrl_spdfb.h"
#include "ctrl_zlzh.h"
#include "section.h"
#include "tim.h"
BSS_CCMRAM control_impl ctrl_impl;
BSS_CCMRAM control ctrl(&ctrl_impl);
ctrl_do_nothing ctrlDoNothing;
ctrl_adda_test ctrlAddaTest;
ctrl_pos_pid ctrlPosPid;
ctrl_pos_pid_individ ctrlPosPidIndivid;
ctrl_pos_hinf1 ctrlPosHinf1;
ctrl_pos_hinf2 ctrlPosHinf2;
ctrl_identify::on_x ctrlIdentify_x;
ctrl_identify::on_y ctrlIdentify_y;
ctrl_identify::on_z ctrlIdentify_z;
ctrl_spdfb1 spdfb1;
ctrl_spdfb2 spdfb2p0;
ctrl_spdfb2p1 spdfb2p1;
ctrl_spdfb2p2 spdfb2p2;
ctrl_spdfb2p3 spdfb2p3;
ctrl_zlzh_p0_s0 zlzh_s0;
ctrl_zlzh_p0_s1 zlzh_s1;

control::pair control::algorithmIntance[] = {
        {"do_nothing",      &ctrlDoNothing,     "什么都不干"},
        {"adda_test",       &ctrlAddaTest,      "ADC DAC测试模式"},
        {"pos_pid",         &ctrlPosPid,        "位置反馈PID控制"},
        {"pos_pid_individ", &ctrlPosPidIndivid, "位置反馈PID控制(不解耦)"},
        {"pos_hinf1",       &ctrlPosHinf1,      "Hinf控制器1"},
        {"pos_hinf2",       &ctrlPosHinf2,      "Hinf控制器2(好使)"},
        {"identify_x",      &ctrlIdentify_x,    "在X向辨识(请从pos_pid切换于此)"},
        {"identify_y",      &ctrlIdentify_y,    "在Y向辨识(请从pos_pid切换于此)"},
        {"identify_z",      &ctrlIdentify_z,    "在Z向辨识(请从pos_pid切换于此)"},
        {"spdfb1",          &spdfb1,            "速度反馈控制器1"},
        {"spdfb2p0",        &spdfb2p0,          "速度反馈控制器2 p0"},
        {"spdfb2p1",        &spdfb2p1,          "速度反馈控制器2"},
        {"spdfb2p2",        &spdfb2p2,          "速度反馈控制器2"},
        {"spdfb2p3",        &spdfb2p3,          "速度反馈控制器2"},
        {"zlzh_s0",         &zlzh_s0,           "互补控制器策略0"},
        {"zlzh_s1",         &zlzh_s1,           "互补控制器策略1"},
        {nullptr,           nullptr,            nullptr}};

control::control(control_impl *impl_) {
    impl = impl_;
}
control::~control() = default;

/// \brief 控制服务的初始化函数，可以在这里初始化一些变量
void control::Init() {
    auto algo = algorithmIntance; // Init所有控制算法
    while (algo->intanceStr != nullptr) {
        algo->intance->Init();
        algo++;
    }
    AlgorithmSwitch("do_nothing");
    HAL_TIM_Base_Start_IT(&htim13); // 打开周期中断的时间基准
}

/// \brief 控制主服务程序，
void control::Process() const {
    brd.htim_counter_data[3] = __HAL_TIM_GET_COUNTER(brd.htim_counter);
    Convst();
    brd.htim_counter_data[4] = __HAL_TIM_GET_COUNTER(brd.htim_counter);
    if (m_algorithmCurrent != nullptr)
        m_algorithmCurrent->Process();
    brd.htim_counter_data[5] = __HAL_TIM_GET_COUNTER(brd.htim_counter);
    Output();
    brd.htim_counter_data[6] = __HAL_TIM_GET_COUNTER(brd.htim_counter);
}

bool control::AlgorithmSwitch(const char *algo_str) {
    algorithm *intance_next = FindAlgorithm(algo_str);
    if (intance_next != nullptr) {
        if (m_algorithmCurrent != nullptr)
            m_algorithmCurrent->LeaveCallBack();
        intance_next->EnterCallBack();
        m_algorithmCurrent = intance_next;
        return true;
    }
    return false;
}

control::algorithm *control::FindAlgorithm(const char *algo_str) {
    algorithm *rtv = nullptr;
    auto algo = algorithmIntance;
    while (algo->intanceStr != nullptr) {
        if (strcmp(algo->intanceStr, algo_str) == 0) {
            rtv = algo->intance;
            break;
        }
        algo++;
    }
    return rtv;
}
RAM_FUNC void control::Convst() const {
    /*
     * ADC通道映射
     * (adc通道和dac通道都是本工程中的逻辑编号)
     *
     * adc通道(从1计数) —— SMA板编号 —— 传感器(编号) —— 线缆编号
     *  4              ——           ——  位移(1 y1) —— 1
     *  8              ——           ——  位移(2 x1) —— 2
     *  7              ——           ——  位移(3 y2) —— 3
     *  6              ——           ——  位移(4 x2) —— 4
     *  6              ——           ——  941b(1 ) —— x
     *  6              ——           ——  941b(2 ) —— x
     *  6              ——           ——  941b(3 ) —— x
     *  6              ——           ——  941b(4 ) —— x
     *  6              ——           ——  941b(5 ) —— x
     *  6              ——           ——  941b(6 ) —— x
     */
    impl->sensor_cd22_raw[0] = control_impl::adc_to_s * float(brd.adc0[3]);
    impl->sensor_cd22_raw[1] = control_impl::adc_to_s * float(brd.adc0[7]);
    impl->sensor_cd22_raw[2] = control_impl::adc_to_s * float(brd.adc0[6]);
    impl->sensor_cd22_raw[3] = control_impl::adc_to_s * float(brd.adc0[5]);

    impl->sensor_cd22_lowpass = 0.999f * impl->sensor_cd22_lowpass + 0.001f * impl->sensor_cd22_raw;

    impl->sensor_cd22 = impl->sensor_cd22_raw - impl->sensor_cd22_bias;

    UPLOAD_DATA_Ix(0) = impl->sensor_cd22[0];
    UPLOAD_DATA_Ix(1) = impl->sensor_cd22[1];
    UPLOAD_DATA_Ix(2) = impl->sensor_cd22[2];
    UPLOAD_DATA_Ix(3) = impl->sensor_cd22[3];

    impl->sensor_cd22_xyz = impl->Tss_mat * impl->sensor_cd22;
    UPLOAD_DATA_Ix(8) = impl->sensor_cd22_xyz[0];
    UPLOAD_DATA_Ix(9) = impl->sensor_cd22_xyz[1];
    UPLOAD_DATA_Ix(10) = impl->sensor_cd22_xyz[2];

    impl->sensor_941b_raw[0] = control_impl::adc_to_941b[0] * brd.adc1[5];
    impl->sensor_941b_raw[1] = control_impl::adc_to_941b[1] * brd.adc1[4];
    impl->sensor_941b_raw[2] = control_impl::adc_to_941b[2] * brd.adc1[2];
    impl->sensor_941b_raw[3] = control_impl::adc_to_941b[3] * brd.adc1[3];

    impl->sensor_941b_low[0] = control_impl::adc_to_941b[0] * brd.adc0[2];//19613
    impl->sensor_941b_low[1] = control_impl::adc_to_941b[0] * brd.adc1[7];//19612
    impl->sensor_941b_low[2] = control_impl::adc_to_941b[0] * brd.adc1[7];//19607
    impl->sensor_941b_low[3] = control_impl::adc_to_941b[0] * brd.adc0[2];//19608

    impl->sensor_941b_lowpass = 0.999f * impl->sensor_941b_lowpass + 0.001f * impl->sensor_941b_raw;
    impl->sensor_941b = impl->sensor_941b_raw - impl->sensor_941b_bias;

    UPLOAD_DATA_Ix(14) = impl->sensor_941b[0];
    UPLOAD_DATA_Ix(15) = impl->sensor_941b[1];
    UPLOAD_DATA_Ix(16) = impl->sensor_941b[2];
    UPLOAD_DATA_Ix(17) = impl->sensor_941b[3];
    UPLOAD_DATA_Ix(18) = impl->sensor_941b_low[0];
    UPLOAD_DATA_Ix(19) = impl->sensor_941b_low[1];

    impl->sensor_941b_xyz = impl->Tsv_mat * impl->sensor_941b;
    UPLOAD_DATA_Ix(20) = impl->sensor_941b_xyz[0];
    UPLOAD_DATA_Ix(21) = impl->sensor_941b_xyz[1];
    UPLOAD_DATA_Ix(22) = impl->sensor_941b_xyz[2];
}
RAM_FUNC void control::Output() const {

    UPLOAD_DATA_Ix(4) = impl->ta115_LAC08[0];
    UPLOAD_DATA_Ix(5) = impl->ta115_LAC08[1];
    UPLOAD_DATA_Ix(6) = impl->ta115_LAC08[2];
    UPLOAD_DATA_Ix(7) = impl->ta115_LAC08[3];
    UPLOAD_DATA_Ix(11) = impl->ta115_LAC08_xyz[0];
    UPLOAD_DATA_Ix(12) = impl->ta115_LAC08_xyz[1];
    UPLOAD_DATA_Ix(13) = impl->ta115_LAC08_xyz[2];
    /*
     * dac通道映射
     *
     * dac通道（从0计数） —— SMA板编号 —— 电机机柜端子号 —— 电机编号 —— 电机线编号
     *    8             ——  x        ——  1           ——   (1 y1)    —— 0
     *    9             ——  x        ——  2           ——   (4 x2)    —— 1
     *    10             ——  x        ——  3          ——   (2 x1)    —— 2
     *    11             ——  x        ——  4          ——   (3 y2)    —— 3
     *
     */

    impl->ta115_LAC08_output = impl->ta115_LAC08 + impl->ta115_LAC08_bias; //
    impl->ta115_LAC08_lowpass = 0.999f * impl->ta115_LAC08_lowpass + 0.001f * impl->ta115_LAC08_output;

    // 输出给DAC
    brd.dac0[8] = int(control_impl::f_to_dac * impl->ta115_LAC08_output[0]);
    brd.dac0[10] = int(control_impl::f_to_dac * impl->ta115_LAC08_output[1]);
    brd.dac0[11] = int(control_impl::f_to_dac * impl->ta115_LAC08_output[2]);
    brd.dac0[9] = int(control_impl::f_to_dac * impl->ta115_LAC08_output[3]);
}
void control::set_motor_chnnl_output(int chnnl, float val) {
    ctrl.impl->ta115_LAC08[chnnl] = val;
}
void control::sensor_zero() {
    ctrl.impl->sensor_cd22_bias = ctrl.impl->sensor_cd22_lowpass;
    ctrl.impl->sensor_941b_bias = ctrl.impl->sensor_941b_lowpass;
}
void control::motor_zero() {
    ctrl.impl->ta115_LAC08_bias = ctrl.impl->ta115_LAC08_lowpass;
}
void control::algorithm::LeaveCallBack() // 切出此算法会调用
{
    ctrl.impl->ta115_LAC08.setZero();
    ctrl.impl->ta115_LAC08_xyz.setZero();
}
void control::algorithm::EnterCallBack() // 切到此算法会调用
{
    ctrl.impl->ta115_LAC08.setZero();
    ctrl.impl->ta115_LAC08_xyz.setZero();
}
