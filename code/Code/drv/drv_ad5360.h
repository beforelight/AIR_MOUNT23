#ifndef CAPTURE_CONTROL_DRV_AD5630_H
#define CAPTURE_CONTROL_DRV_AD5630_H
#include "drv_io.h"
class ad5360 {
    class ad5360_chnnl // 起一个语法糖作用
    {
    public:
        explicit ad5360_chnnl(ad5360 *adc) : m_adc(adc) {}
        inline void SetChannel(int chnnl) { m_chnnl = chnnl; }
        //    ad5360_chnnl &operator=(int16_t code);
        ad5360_chnnl &operator=(int32_t code);

    private:
        ad5360 *m_adc;
        int m_chnnl = 0;
    };

public:
    IO m_reset_n;
    IO m_busy;
    IO m_clr_n;
    IO m_ldac_n;
    IO m_sync_n;
    SPI_HandleTypeDef *m_spi;
    uint8_t m_buf[3] = {0};
    ad5360_chnnl m_ad5360Chnnl{this};

    int16_t *m_data = nullptr; // 记录本次DAC数据的缓存

public:
    ad5360(
            IO reset_n,
            IO busy,
            IO clr_n,
            IO ldac_n,
            IO sync_n,
            SPI_HandleTypeDef *spi) :
            m_reset_n(reset_n),
            m_busy(busy),
            m_clr_n(clr_n),
            m_ldac_n(ldac_n),
            m_sync_n(sync_n), m_spi(spi) {}

    void Init();
    void SetBuf(int16_t *pData) { m_data = pData; }
    // 转换函数入口
    void ConvertI16(int chnnl, int16_t code);
    void Convert(int chnnl, int32_t code) {
        if (code > INT16_MAX)
            ConvertI16(chnnl, (int16_t) INT16_MAX);
        else if (code < INT16_MIN)
            ConvertI16(chnnl, (int16_t) INT16_MIN);
        else
            ConvertI16(chnnl, (int16_t) code);
    }
    ad5360_chnnl &operator[](int chnnl);
};
#endif // CAPTURE_CONTROL_DRV_AD5630_H
