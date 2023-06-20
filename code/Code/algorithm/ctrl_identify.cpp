#include "ctrl.hpp"
#include "ctrl_impl.hpp"

namespace ctrl_identify {
    class base : public control::algorithm {
    public:
        void Init() override {}
    };

    #define chirp_fstart (0.01f)
    #define chirp_fend (100.0f)
    #define chirp_tstart (0.0f) //开始必须为0
    #define chirp_tend (40.0f)
    class chirp_bas : public base {
    protected:
        //Vector4f out_bias;//是ta115_LAC08的平均值，用来保证切入的那一刻是无能量的
        float t;
        float t_square;
        void EnterCallBack() override {
            t = 0;
            t_square = 0;
            //out_bias = ctrl.impl->ta115_LAC08;
        }
        float chirp() {
            auto freq = ((chirp_fend - chirp_fstart) / (2.718281f - 1.0f)) * (expf(t / chirp_tend) - 1.0f) + chirp_fstart;
            float k = 1.0f;
            //在1Hz到40Hz之间均匀加40倍
            if (freq > 40.0f)
                k = 40;
            else if (freq > 1.0f)
                k = ((40.0f - 1.0f) / (40.0f - 1.0f)) * (freq - 1.0f) + 1.0f;
            t += (1.0f / (float) brd.ctrl_freq.val);
            if (t > chirp_tend) { t = 0; }
            return k * sinf(2.0f * 3.14f * freq * t);
        }
        float square() {
            t_square += (1.0f / (float) brd.ctrl_freq.val);
            if (t_square > chirp_tend * 0.7f) { t_square = 0; }
            return t_square < chirp_tend * 0.35f ? 1.0f : -1.0f;
        }
    };


    class chirp_on_x : public chirp_bas {
    public:
        void Process() override {
            ctrl.impl->ta115_LAC08_xyz[0] = 0.025f * chirp() + 0.05f * square();
            // 转换到 原始坐标
            ctrl.impl->ta115_LAC08 = ctrl.impl->Ta_mat * ctrl.impl->ta115_LAC08_xyz;
//            ctrl.impl->ta115_LAC08 += out_bias;
        }
    };

    class chirp_on_y : public chirp_bas {
    public:
        void Process() override {
            ctrl.impl->ta115_LAC08_xyz[1] = 0.025f * chirp() + 0.05f * square();
            // 转换到 原始坐标
            ctrl.impl->ta115_LAC08 = ctrl.impl->Ta_mat * ctrl.impl->ta115_LAC08_xyz;
//            ctrl.impl->ta115_LAC08 += out_bias;
        }
    };

    class chirp_on_z : public chirp_bas {
    public:
        void Process() override {
            ctrl.impl->ta115_LAC08_xyz[2] = 0.0035f * chirp() + 0.015f * square();
            // 转换到 原始坐标
            ctrl.impl->ta115_LAC08 = ctrl.impl->Ta_mat * ctrl.impl->ta115_LAC08_xyz;
//            ctrl.impl->ta115_LAC08 += out_bias;
        }
    };

}; // namespace ctrl_identify


ctrl_identify::chirp_on_x ctrlChirpIdentify_x;
ctrl_identify::chirp_on_y ctrlChirpIdentify_y;
ctrl_identify::chirp_on_z ctrlChirpIdentify_z;

CTRL_EXPORT_ALGO(ctrlChirpIdentify_x, "identify_x", "在X向辨识(请从零状态切换于此)");
CTRL_EXPORT_ALGO(ctrlChirpIdentify_y, "identify_y", "在Y向辨识(请从零状态切换于此)");
CTRL_EXPORT_ALGO(ctrlChirpIdentify_z, "identify_z", "在Z向辨识(请从零状态切换于此)");
