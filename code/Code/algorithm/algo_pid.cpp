//
// Created by 17616 on 2022/10/12.
//

#include "algo_pid.h"
void algo_pid::update() {
    // 更新控制
    err0 = ref - fb;
    float err_diff01 = err0 - err1;
    float err_diff12 = err1 - err2;
    float delta_u = kp * err_diff01
                    + ki * Ts * err0
                    + kd * Ts_inver * (err_diff01 - err_diff12);
    u += delta_u;
    // 差分
    err2 = err1;
    err1 = err0;
    // 限幅
    if (u > u_max) {
        u = u_max;
    }
    if (u < u_min) {
        u = u_min;
    }
}
std::string algo_pid::str() {
    std::string str;
    str += my::to_string(kp);
    str += '\t';
    str += my::to_string(ki);
    str += '\t';
    str += my::to_string(kd);
    return str;
}

void algo_pid::scan(const std::string &str) {
    std::vector<std::string> split;
    Stringsplit(str, '\t', split);
    if (split.size() > 2)
        kd = std::stof(split[2]);
    if (split.size() > 1)
        ki = std::stof(split[1]);
    if (!split.empty())
        kp = std::stof(split[0]);
}
void algo_pid::clear() {
    err0 = 0;
    err1 = 0;
    err2 = 0;
    u = 0;
}

float algo_downSample::update(float v) {
    out = out * (1 - tau) + tau * v;
    return out;
}
algo_downSample::algo_downSample(float tau_) : tau(tau_) {}
