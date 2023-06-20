#include "drv_ad7606.h"
#pragma GCC optimize ("Ofast")
void ad7606::StartConvert()
{
  m_convsta = 0;
//  __NOP();
  m_convsta = 1;
}
void ad7606::Init()
{
  m_cs_n = 1;
  m_convsta = 1;
  m_reset = 1;

  HAL_Delay(1);
  m_reset = 0;
  HAL_Delay(1);
  IsOpen = true;
}
void ad7606::StartOfRead(int16_t *pOspBuf)
{
  if (IsOpen) {
    m_cs_n = 0;
    //    HAL_SPI_TransmitReceive(m_spi, (uint8_t *) (m_data + 8), (uint8_t *) m_data, 8, 2);
    //    HAL_SPI_RxCpltCallback(m_spi); // 兼容DMA模式
    HAL_SPI_Receive_DMA(m_spi, (uint8_t *) pOspBuf, 8); // 会一直出现overrun错误
  }
}
void ad7606::EndOfRead()
{
  m_cs_n = 1;
}

void ad7606::StopRead()
{
  EndOfRead();
  HAL_SPI_DMAStop(m_spi);
}

void ad7606::EndOfOversample(int16_t *pData)
{
  for (int j = 0; j < 8; ++j) {
    int tmp = 0;
    for (int i = 0; i < m_osp; ++i) {
      tmp += m_osp_buf[i * 8 + j]; // 先计算每个通道
    }
    //    pData[j] = tmp / m_osp;
    pData[j] = (tmp > 0 ? tmp + m_osp_half : tmp - m_osp_half) / m_osp; // 四舍五入提高精度
  }
  m_data = pData;
}
