#include "board.hpp"
#include "ctrl.hpp"
class ctrl_adda_test : public control::algorithm {
public:
    void Process() override // 控制程序
    {
        int tim_counter_idx = 0;
        const float sin_table[17] = {
                0, 361.2417, 673.6956, 895.1633, 995.7342, 961.8256, 798.0172, 526.4322, 183.7495,
                -183.7495, -526.4322, -798.0172,
                -961.8256, -995.7342, -895.1633, -673.6956, -361.2417};
        static int idx = 0;

        for (int i = 0; i < 16; ++i) {
            brd.dac[i] = sin_table[idx];
            idx++;
            idx %= 17;
        }
    }
    void Init() override {}          // 初始化等
};


class ctrl_adda_test2 : public control::algorithm {
public:
    void Process() override // 控制程序
    {
        int tim_counter_idx = 0;
        const int16_t sin_table[17] = {
                1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000,
                0, 0, 0, 0, 0, 0, 0, 0};
        static int idx = 0;

        for (int i = 0; i < 16; ++i) {
            brd.dac[i] = sin_table[idx];
            idx++;
            idx %= 17;
        }
    }
    void Init() override {}          // 初始化等
};

ctrl_adda_test adda_test;
ctrl_adda_test2 adda_test2;

CTRL_EXPORT_ALGO(adda_test, "adda_test", "ADC DAC测试");
CTRL_EXPORT_ALGO(adda_test2, "adda_test2", "ADC DAC测试");
