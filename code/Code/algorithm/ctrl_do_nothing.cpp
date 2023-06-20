#include "ctrl.hpp"
class ctrl_do_nothing : public control::algorithm {
public:
    void Process() override {}       // 控制程序
    void Init() override {}          // 初始化等
};
ctrl_do_nothing do_nothing;
CTRL_EXPORT_ALGO(do_nothing,"do_nothing", "什么都不干");