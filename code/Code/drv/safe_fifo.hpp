#ifndef CAPTURE_CONTROL_SAFE_FIFO_H
#define CAPTURE_CONTROL_SAFE_FIFO_H
#include "safe_lock.hpp"
#include <algorithm>
#include <stdint.h>
template <class Tp, int Bits>
class safe_fifo
{
public:
  // 写入buff，返回实际读写大小
  uint32_t Put(Tp *src, uint32_t len)
  {
    safe_lock lock;
    len = std::min(len, m_size - m_in + m_out); // 满了之后就不会再接收了
    uint32_t l = std::min(len, m_size - m_in);
    memcpy(m_buffer + m_in, src, l * sizeof(Tp));
    memcpy(m_buffer, src + l, (len - l) * sizeof(Tp));
    m_in = (m_in + len) & m_size_mask;
    return len;
  }
  // 读出buff，返回实际读写大小
  uint32_t Get(Tp *dst, uint32_t len)
  {
    safe_lock lock;
    len = std::min(len, m_in - m_out);
    uint32_t l = std::min(len, m_size - m_out);
    memcpy(dst, m_buffer + m_out, l * sizeof(Tp));
    memcpy(dst + l, m_buffer, (len - l) * sizeof(Tp));
    m_out = (m_out + len) & m_size_mask;
    return len;
  }
  // buff是否为空
  bool empty() { return m_out == m_in; }
  // 环形buff大小
  uint32_t Size(void) { return m_size; }
  // 已使用大小
  uint32_t UsedSize(void) { return ((m_size + m_in - m_out) & m_size_mask); }
  // 未使用大小
  uint32_t UnusedSize(void) { return ((m_size + m_out - m_in) & m_size_mask); }
  // 清空buff
  void Clear(void) { m_out = m_in; }

  /* 以下为0拷贝API*/
  // 头入尾出，返回头
  Tp &head() { return m_buffer[m_in]; }
  // 头入尾出，返回尾
  Tp &tail() { return m_buffer[m_out]; }
  // 弹出尾巴
  void pop_tail() { m_out = (++m_out) & m_size_mask; }
  void pop_tail(uint32_t len) { m_out = (m_out + len) & m_size_mask; }
  // 压入头
  void put_head() { m_in = (++m_in) & m_size_mask; }
  void put_head(uint32_t len) { m_in = (m_in + len) & m_size_mask; }
  /* 0拷贝API*/

public:
  uint32_t m_size = 1 << Bits;
  uint32_t m_size_mask = (1 << Bits) - 1;
  volatile uint32_t m_in = 0;  // addr of buffer to write in
  volatile uint32_t m_out = 0; // addr of buffer to read out
  Tp m_buffer[1 << Bits];
};
#endif // CAPTURE_CONTROL_SAFE_FIFO_H
