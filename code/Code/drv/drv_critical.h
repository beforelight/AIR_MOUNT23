#ifndef T12PEN_CRITICAL_H
#define T12PEN_CRITICAL_H
#include"cmsis_compiler.h"
class critical {
public:
    critical() {
        priMask = __get_PRIMASK();
        __disable_irq();
    }
    ~critical() { __set_PRIMASK(priMask); }

    critical(const critical &) = delete;
    critical(critical &&) = delete;
    critical &operator=(const critical &) = delete;
    critical &operator=(critical &&) = delete;
private:
    uint32_t priMask;
};

#endif //T12PEN_CRITICAL_H
