#include "ctrl.hpp"
#include "ctrl_impl.hpp"

control_impl ctrl_impl;
control ctrl(&ctrl_impl);

void control::Process() const {
    brd.htim_counter_data[3] = __HAL_TIM_GET_COUNTER(brd.htim_counter);
    impl->Convst();
    brd.htim_counter_data[4] = __HAL_TIM_GET_COUNTER(brd.htim_counter);
    if (m_algorithmCurrent != nullptr)
        m_algorithmCurrent->Process();
    brd.htim_counter_data[5] = __HAL_TIM_GET_COUNTER(brd.htim_counter);
    impl->Output();
    brd.htim_counter_data[6] = __HAL_TIM_GET_COUNTER(brd.htim_counter);
}

void control::Init() {
    impl->Init();
    for (auto i = pair::begin(); i != pair::end(); ++i) {
        i->intance->Init();//初始化所有
    }
    AlgorithmSwitch("do_nothing");
}
bool control::AlgorithmSwitch(const char *algo_str) {
    auto pair = control::pair::find(algo_str);
    if (pair != nullptr) {
        if (m_algorithmCurrent != nullptr)
            m_algorithmCurrent->LeaveCallBack();
        algorithm *intance_next = pair->intance;
        intance_next->EnterCallBack();
        m_algorithmCurrent = intance_next;
        return true;
    }
    return false;
}

void control::set_motor_chnnl_output(int chnnl, float val) {
    impl->ta115_LAC08[chnnl] = val;
}
void control::set_motor_xyz_output(int chnnl, float val) {
    impl->ta115_LAC08_xyz[chnnl] = val;
    impl->ta115_LAC08 = impl->Ta_mat * impl->ta115_LAC08_xyz;
}
void control::sensor_zero() {
    impl->sensor_cd22_bias = impl->sensor_cd22_lowpass;
}
void control::sensor_zero_for_941b() {
    impl->sensor_941b_bias = impl->sensor_941b_lowpass;
    impl->base_sensor_941b_bias = impl->base_sensor_941b_lowpass;
}
void control::motor_zero() {
    impl->ta115_LAC08_bias = impl->ta115_LAC08_lowpass;
}
void control::after_par_load() {
    impl->sensor_cd22_lowpass = impl->sensor_cd22_bias;
    impl->sensor_941b_lowpass = impl->sensor_941b_bias;
    impl->base_sensor_941b_lowpass = impl->base_sensor_941b_bias;
    impl->ta115_LAC08_lowpass = impl->ta115_LAC08_bias;
}

control::control(control_impl *impl_) : impl(impl_) {}


control::~control() = default;

void control::algorithm::EnterCallBack() {
    ctrl.impl->ta115_LAC08.setZero();
    ctrl.impl->ta115_LAC08_xyz.setZero();
}
void control::algorithm::LeaveCallBack() {
    ctrl.impl->ta115_LAC08.setZero();
    ctrl.impl->ta115_LAC08_xyz.setZero();
}
const control::pair *control::pair::begin() {
    extern const unsigned int _ctrl_pair_start;
    return (const control::pair *) &_ctrl_pair_start;
}
const control::pair *control::pair::end() {
    extern const unsigned int _ctrl_pair_end;
    return (const control::pair *) &_ctrl_pair_end;
}
const control::pair *control::pair::find(control::algorithm *instance) {
    const control::pair *rtv = nullptr;
    for (auto i = begin(); i != end(); ++i) {
        if (i->intance == instance) {
            rtv = i;
            break;
        }
    }
    return rtv;
}
const control::pair *control::pair::find(const char *intanceStr) {
    const control::pair *rtv = nullptr;
    for (auto i = begin(); i != end(); ++i) {
        if (strcmp(i->intanceStr, intanceStr) == 0) {
            rtv = i;
            break;
        }
    }
    return rtv;
}
