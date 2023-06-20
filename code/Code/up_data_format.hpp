#ifndef CAPTURE_CONTROL_UP_DATA_FORMAT_H
#define CAPTURE_CONTROL_UP_DATA_FORMAT_H
#include "safe_fifo.hpp"
#include <cstdint>
#define UP_DATA_FLOAT_COUNT (24) // 如果要发送更多的浮点数，调整这个值

/// \brief
/// \tparam Tp
/// \tparam N 必须等于(1<<bits)
template<class Tp, int N>
class triPtrFifo : public safe_fifo<Tp, N> {
public:
    template<int Nb>
    explicit triPtrFifo(Tp (&buffer)[Nb]):safe_fifo<Tp, N>(buffer) {
        static_assert(N == Nb, "检查数组尺寸");
    }
    volatile uint32_t m_using = 0;
};


struct up_data_format {
    uint16_t pkg_int16 = 0xa5a5;
    uint16_t pkg_len = sizeof(up_data_format);
    uint16_t pkg_float_offset;              // 开始记录浮点数据的偏移
    uint16_t pkg_idx = 0;                   // 记录这个包裹的id，一般是内存序号
    int16_t adc_buf[3][8]{0};               // ADC通道一共24个
    int16_t dac_buf[1][16]{0};              // DAC通道一共16个
    float flt_data[UP_DATA_FLOAT_COUNT]{0}; // 浮点数据之前就有32个数据通道
    uint32_t pkg_end = 0x5a5a5a5a;
    up_data_format() {
        pkg_float_offset = ((uint32_t) (&flt_data)) - ((uint32_t) (this));
    }
} __attribute__((aligned(4)));

extern triPtrFifo<up_data_format, 64> updata_fifo;

/// 这里的<idx>和上位机显示的通道号一致，上传数据缓存
#define UPLOAD_DATA_Ix(idx)                                           \
      static_assert(0 <= idx && idx < (UP_DATA_FLOAT_COUNT), "限制通道"); \
      (updata_fifo.head().flt_data[idx])

/// 这里的<idx>和上位机显示的通道号一致，但不带静态检查
#define UPLOAD_DATA_Ix_NOCHECK(idx) \
      (updata_fifo.head().flt_data[idx])

#endif // CAPTURE_CONTROL_UP_DATA_FORMAT_H
