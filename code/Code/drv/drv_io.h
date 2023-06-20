
#ifndef CAPTURE_CONTROL_DRV_IO_H
#define CAPTURE_CONTROL_DRV_IO_H
#include "stm32f4xx_hal.h"
class IO {
public:
    GPIO_TypeDef *m_GPIOx = nullptr;
    uint16_t m_GPIO_Pin = 0;

public:
    IO() = default;
    IO(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) : m_GPIOx(GPIOx), m_GPIO_Pin(GPIO_Pin) {}

public:
    inline IO &operator=(bool set) {
        if (isOpen())
            HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, set ? GPIO_PIN_SET : GPIO_PIN_RESET);
        return *this;
    }

    inline IO &operator=(int v) {
        if (isOpen())
            HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, v > 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
        return *this;
    }

    inline int operator()() {
        if (!isOpen())
            return 0;
        return HAL_GPIO_ReadPin(m_GPIOx, m_GPIO_Pin) == GPIO_PIN_SET ? 1 : 0;
    }
    inline bool isOpen() { return m_GPIOx != nullptr; }
};
#endif // CAPTURE_CONTROL_DRV_IO_H
