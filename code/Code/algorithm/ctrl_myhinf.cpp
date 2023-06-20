#include "algo_pid.h"
#include "iir_df2tf.hpp"
#include "iir_df2sos_new.hpp"
#include "ctrl.hpp"
#include "ctrl_impl.hpp"

namespace my {
    class hinf_allfb : public control::algorithm {
    public:
        algo_pid pid[3];
        iir_df2sos<float, 6> hinf[3] = {
                {
                        {
                                1, -1.98023775839268, 0.980320732826287, 1, -1.81331359969253, 0.83679015124794,
                                1, -0.41827930553933, -0.330485788651052, 1, -1.99909643343215, 0.999096789228853,
                                0, 1, 0,                  1, -0.870578595939936, 0
                        },
                        0.00384391505166567
                },
                {
                        {
                                1, -1.98023775839268, 0.980320732826287, 1, -1.81331359969253, 0.83679015124794,
                                1, -0.41827930553933, -0.330485788651052, 1, -1.99909643343215, 0.999096789228853,
                                0, 1, 0,                  1, -0.870578595939936, 0
                        },
                        0.00384391505166567
                },
                {
                        {
                                1, -1.97489276132536, 0.974965145784407, 1, -1.60140558021793, 0.636197169959634,
                                1, -1.75293731347329, 0.761994320836541,  1, -1.97142881031372, 0.971602465235901,
                                0, 1, -0.991452725496459, 1, -1.99846880079249,  0.998469068182444
                        },
                        0.00173842982489001
                }
        };
    public:
        void Init() override {
            pid[0] = {1.0f / (float) brd.ctrl_freq.val, 0.5, -0.5, "hinf_allfb"};
            pid[0].ki = 0.00778393603811692;
            pid[1] = {1.0f / (float) brd.ctrl_freq.val, 0.5, -0.5, "hinf_allfb"};
            pid[1].ki = 0.00778393603811692;
            pid[2] = {1.0f / (float) brd.ctrl_freq.val, 0.125, -0.125, "hinf_allfb"};
            pid[2].ki = 0.000858932644805886;
        }
        void Process() override {
            //计算pid
            pid[0].fb = ctrl.impl->sensor_cd22_xyz[0];
            pid[1].fb = ctrl.impl->sensor_cd22_xyz[1];
            pid[2].fb = ctrl.impl->sensor_cd22_xyz[2];
            pid[0].update();
            pid[1].update();
            pid[2].update();

            // 计算hinf
            ctrl.impl->ta115_LAC08_xyz[0] = hinf[0].update(-ctrl.impl->sensor_941b_xyz[0]) + pid[0].u;
            ctrl.impl->ta115_LAC08_xyz[1] = hinf[1].update(-ctrl.impl->sensor_941b_xyz[1]) + pid[1].u;
            ctrl.impl->ta115_LAC08_xyz[2] = hinf[2].update(-ctrl.impl->sensor_941b_xyz[2]) + pid[2].u;

            // 转换到 原始坐标
            ctrl.impl->ta115_LAC08 = ctrl.impl->Ta_mat * ctrl.impl->ta115_LAC08_xyz;
        }
    };


    class hinf_allfb_onlyspd : public hinf_allfb {
    public:
        void Process() override {
            // 计算hinf
            ctrl.impl->ta115_LAC08_xyz[0] = hinf[0].update(-ctrl.impl->sensor_941b_xyz[0]);
            ctrl.impl->ta115_LAC08_xyz[1] = hinf[1].update(-ctrl.impl->sensor_941b_xyz[1]);
            ctrl.impl->ta115_LAC08_xyz[2] = hinf[2].update(-ctrl.impl->sensor_941b_xyz[2]);

            // 转换到 原始坐标
            ctrl.impl->ta115_LAC08 = ctrl.impl->Ta_mat * ctrl.impl->ta115_LAC08_xyz;
        }

    };
}



my::hinf_allfb hinf_allfb;
CTRL_EXPORT_ALGO(hinf_allfb, "hinf_allfb", "带位移闭环的速度反馈的hinf无穷控制器");

my::hinf_allfb_onlyspd hinf_allfb_onlyspd;
CTRL_EXPORT_ALGO(hinf_allfb_onlyspd, "hinf_allfb_onlyspd", "带位移闭环的速度反馈的hinf无穷控制器，关闭位移环控制，实验用");