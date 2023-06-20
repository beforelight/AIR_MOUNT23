#include "drv_ad5360.h"
#include <cassert>
#pragma GCC optimize ("Ofast")
void ad5360::Init()
{
  m_sync_n = 1;
  m_ldac_n = 0;
  m_clr_n = 1;
  m_reset_n = 0;
  HAL_Delay(1);
  m_reset_n = 1;
  HAL_Delay(1);
}
void ad5360::ConvertI16(int chnnl, int16_t code)
{
//  assert(chnnl >= 0 && chnnl < 16);
  m_data[chnnl] = code; // 记录到缓存中
  chnnl += 0b1000;
  (*(uint16_t *) (&code)) += 0x8000; // 其内部不支持补码
  m_buf[0] = (0b11000000) | (chnnl & 0b00111111);
  m_buf[1] = (*(uint16_t *) (&code)) >> 8; // 高8位
  m_buf[2] = (*(uint16_t *) (&code));      // 低8位

  m_sync_n = 0;
  // HAL api
  HAL_SPI_Transmit(m_spi, m_buf, 3, 10);

  m_sync_n = 1;
}
ad5360::ad5360_chnnl &ad5360::operator[](int chnnl)
{
  m_ad5360Chnnl.SetChannel(chnnl);
  return m_ad5360Chnnl;
}
//ad5360::ad5360_chnnl &ad5360::ad5360_chnnl::operator=(int16_t code)
//{
//  m_adc->Convert(m_chnnl, code);
//  return *this;
//}
ad5360::ad5360_chnnl &ad5360::ad5360_chnnl::operator=(int32_t code)
{
  m_adc->Convert(m_chnnl, code);
  return *this;
}
