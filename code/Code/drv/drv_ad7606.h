#ifndef CAPTURE_CONTROL_DRV_AD7606_H
#define CAPTURE_CONTROL_DRV_AD7606_H
#include "drv_io.h"

class ad7606
{
public:
  IO m_reset;
  IO m_convsta;
  IO m_cs_n;
  IO m_busy; // 中断引脚,下降沿转换完成
  SPI_HandleTypeDef *m_spi;

  int16_t *m_data = nullptr; // 接收完毕的缓存地址
                             //  int16_t *m_data_recv = nullptr; // 正在接收的缓存地址
  bool IsOpen = false;
  int16_t *m_osp_buf; // 过采样缓存,由外部提供
  int m_osp;          // 过采样次数
  int m_osp_half;     // 过采样次数的一半
public:
  ad7606(
      IO reset,
      IO convsta,
      IO cs_n,
      IO busy,
      SPI_HandleTypeDef *spi,
      int16_t *over_sample_buf,
      int over_sample = 1)
      : m_reset(reset),
        m_convsta(convsta),
        m_cs_n(cs_n),
        m_busy(busy),
        m_spi(spi),
        m_osp_buf(over_sample_buf),
        m_osp(over_sample),
        m_osp_half(over_sample / 2)
  {
  }
  void Init();
  void StartConvert();
  void StartOfRead(int16_t *pOspBuf);
  void EndOfRead();                     // 传输完成时调用
  void EndOfOversample(int16_t *pData); // 结束过采样
  void StopRead();                // 强行终止传输
  inline int16_t &operator[](int idx) const { return m_data[idx]; }
  SPI_HandleTypeDef *&GetSpi() { return m_spi; }
};

#endif // CAPTURE_CONTROL_DRV_AD7606_H
