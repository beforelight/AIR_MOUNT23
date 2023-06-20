#include "drv_ad7606.h"
void ad7606::StartConvert() {
    m_convsta = 0;
    //  __NOP();
    m_convsta = 1;
}
void ad7606::Init() {
    m_cs_n = 1;
    m_convsta = 1;
    m_reset = 1;

    HAL_Delay(1);
    m_reset = 0;
    HAL_Delay(1);
    IsOpen = true;
}
void ad7606::StartOfRead(int16_t *pOspBuf) {
    if (IsOpen) {
        m_cs_n = 0;
        HAL_SPI_Receive_DMA(m_spi, (uint8_t *) pOspBuf, 8); // 会一直出现overrun错误
        m_data = pOspBuf;
    }
}
void ad7606::EndOfRead() {
    m_cs_n = 1;
}

void ad7606::StopRead() {
    EndOfRead();
    HAL_SPI_DMAStop(m_spi);
}

