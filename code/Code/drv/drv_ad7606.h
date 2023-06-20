#ifndef CAPTURE_CONTROL_DRV_AD7606_H
#define CAPTURE_CONTROL_DRV_AD7606_H
#include "drv_io.h"

class ad7606 {
public:
    IO m_reset;
    IO m_convsta;
    IO m_cs_n;
    IO m_busy; // 中断引脚,下降沿转换完成
    SPI_HandleTypeDef *m_spi;

    int16_t *m_xferBuff = nullptr;//传输缓存
    int16_t *m_data = nullptr; // 接收完毕的缓存地址
    //  int16_t *m_data_recv = nullptr; // 正在接收的缓存地址
    bool IsOpen = false;
public:
    ad7606(
            IO reset,
            IO convsta,
            IO cs_n,
            IO busy,
            SPI_HandleTypeDef *spi) :
            m_reset(reset),
            m_convsta(convsta),
            m_cs_n(cs_n),
            m_busy(busy),
            m_spi(spi) {
    }
    void Init();
    void StartConvert();
    //准备传输缓存
    void PrepareXferBuff(int16_t *xferBuff) { m_xferBuff = xferBuff; }
    //设置有效数据的缓存
    void SetUsingData(int16_t *usingData) { m_data = usingData; }
    void StartOfRead();
    void EndOfRead();                     // 传输完成时调用
    void StopRead();                // 强行终止传输
    inline int16_t &operator[](int idx) const { return m_data[idx]; }
    SPI_HandleTypeDef *&GetSpi() { return m_spi; }
};

#endif // CAPTURE_CONTROL_DRV_AD7606_H
