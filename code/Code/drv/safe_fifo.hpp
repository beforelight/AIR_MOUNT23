#ifndef CAPTURE_CONTROL_SAFE_FIFO_H
#define CAPTURE_CONTROL_SAFE_FIFO_H
#include "safe_lock.hpp"
#include <algorithm>
#include <cstdint>
#include <cstring>
/// \brief
/// \tparam Tp
/// \tparam N 必须等于(1<<bits)
template<class Tp, int N>
class safe_fifo {
public:
    safe_fifo() = default;
    template<int Nb>
    explicit safe_fifo(Tp (&buffer)[Nb]):m_buffer(buffer) {
        static_assert(
                (
                        (N == (1UL << 0)) ||
                        (N == (1UL << 1)) ||
                        (N == (1UL << 2)) ||
                        (N == (1UL << 3)) ||
                        (N == (1UL << 4)) ||
                        (N == (1UL << 5)) ||
                        (N == (1UL << 6)) ||
                        (N == (1UL << 7)) ||
                        (N == (1UL << 8)) ||
                        (N == (1UL << 9)) ||
                        (N == (1UL << 10)) ||
                        (N == (1UL << 11)) ||
                        (N == (1UL << 12)) ||
                        (N == (1UL << 13)) ||
                        (N == (1UL << 14)) ||
                        (N == (1UL << 15)) ||
                        (N == (1UL << 16)) ||
                        (N == (1UL << 17)) ||
                        (N == (1UL << 18)) ||
                        (N == (1UL << 19)) ||
                        (N == (1UL << 20)) ||
                        (N == (1UL << 21)) ||
                        (N == (1UL << 22)) ||
                        (N == (1UL << 23)) ||
                        (N == (1UL << 24)) ||
                        (N == (1UL << 25)) ||
                        (N == (1UL << 26)) ||
                        (N == (1UL << 27)) ||
                        (N == (1UL << 28)) ||
                        (N == (1UL << 29)) ||
                        (N == (1UL << 20)) ||
                        (N == (1UL << 31))
                ), "N必须是2的阶数");
        static_assert(N == Nb, "检查数组尺寸");
    }

    // 写入buff，返回实际读写大小
    uint32_t Put(Tp *src, uint32_t len) {
        safe_lock lock;
        len = std::min(len, m_size - m_in + m_out); // 满了之后就不会再接收了
        uint32_t l = std::min(len, m_size - m_in);
        memcpy(m_buffer + m_in, src, l * sizeof(Tp));
        memcpy(m_buffer, src + l, (len - l) * sizeof(Tp));
        m_in = (m_in + len) & m_size_mask;
        return len;
    }
    // 读出buff，返回实际读写大小
    uint32_t Get(Tp *dst, uint32_t len) {
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
    uint32_t Size() { return m_size; }
    // 已使用大小
    uint32_t UsedSize() { return ((m_size + m_in - m_out) & m_size_mask); }
    // 未使用大小
    uint32_t UnusedSize() { return ((m_size + m_out - m_in) & m_size_mask); }
    // 清空buff
    void Clear() { m_out = m_in; }

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
    Tp *m_buffer = nullptr;
    constexpr static uint32_t m_size = N;
    constexpr static uint32_t m_size_mask = (N) - 1;
    volatile uint32_t m_in = 0;  // addr of buffer to write in
    volatile uint32_t m_out = 0; // addr of buffer to read out
};


template<class Tp, int N>
class safe_fifo_with_buff {
public:
    safe_fifo_with_buff() {
        static_assert(
                (
                        (N == (1UL << 0)) ||
                        (N == (1UL << 1)) ||
                        (N == (1UL << 2)) ||
                        (N == (1UL << 3)) ||
                        (N == (1UL << 4)) ||
                        (N == (1UL << 5)) ||
                        (N == (1UL << 6)) ||
                        (N == (1UL << 7)) ||
                        (N == (1UL << 8)) ||
                        (N == (1UL << 9)) ||
                        (N == (1UL << 10)) ||
                        (N == (1UL << 11)) ||
                        (N == (1UL << 12)) ||
                        (N == (1UL << 13)) ||
                        (N == (1UL << 14)) ||
                        (N == (1UL << 15)) ||
                        (N == (1UL << 16)) ||
                        (N == (1UL << 17)) ||
                        (N == (1UL << 18)) ||
                        (N == (1UL << 19)) ||
                        (N == (1UL << 20)) ||
                        (N == (1UL << 21)) ||
                        (N == (1UL << 22)) ||
                        (N == (1UL << 23)) ||
                        (N == (1UL << 24)) ||
                        (N == (1UL << 25)) ||
                        (N == (1UL << 26)) ||
                        (N == (1UL << 27)) ||
                        (N == (1UL << 28)) ||
                        (N == (1UL << 29)) ||
                        (N == (1UL << 20)) ||
                        (N == (1UL << 31))
                ), "N必须是2的阶数");
    }

    // 写入buff，返回实际读写大小
    uint32_t Put(Tp *src, uint32_t len) {
        safe_lock lock;
        len = std::min(len, m_size - m_in + m_out); // 满了之后就不会再接收了
        uint32_t l = std::min(len, m_size - m_in);
        memcpy(m_buffer + m_in, src, l * sizeof(Tp));
        memcpy(m_buffer, src + l, (len - l) * sizeof(Tp));
        m_in = (m_in + len) & m_size_mask;
        return len;
    }
    // 读出buff，返回实际读写大小
    uint32_t Get(Tp *dst, uint32_t len) {
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
    uint32_t Size() { return m_size; }
    // 已使用大小
    uint32_t UsedSize() { return ((m_size + m_in - m_out) & m_size_mask); }
    // 未使用大小
    uint32_t UnusedSize() { return ((m_size + m_out - m_in) & m_size_mask); }
    // 清空buff
    void Clear() { m_out = m_in; }

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
    Tp m_buffer[N];
    constexpr static uint32_t m_size = N;
    constexpr static uint32_t m_size_mask = (N) - 1;
    volatile uint32_t m_in = 0;  // addr of buffer to write in
    volatile uint32_t m_out = 0; // addr of buffer to read out
};
#endif // CAPTURE_CONTROL_SAFE_FIFO_H
