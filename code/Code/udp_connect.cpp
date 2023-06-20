#include "udp_connect.h"
#include "lwip.h"
#include "tcp.h"
#include "up_data_format.hpp"
#include <algorithm>
#include <cassert>
#include <cstdio>
struct udp_pcb *sv_pcb;
ip_addr_t remote_ip;
ip_addr_t local_ip;
volatile bool is_connected = false;
void udp_callback(
        void *arg, struct udp_pcb *pPcb, struct pbuf *p,
        const ip_addr_t *addr, u16_t port) {
    //  udp_connect(sv_pcb, addr, port);
    //  is_connected = true;
    pbuf_free(p);
}

void ConnectInit() {
    // 缓存初始化
    for (int i = 0; i < updata_fifo.m_size; ++i) {
        updata_fifo.m_buffer[i].pkg_idx = i;
    }

    sv_pcb = udp_new();
    assert(sv_pcb != nullptr);
    ip_addr_t dst_ip;
    IP_ADDR4(&dst_ip, 192, 168, 123, 2);
    udp_bind(sv_pcb, IP_ADDR_ANY, 8000);
    udp_connect(sv_pcb, &dst_ip, 8001);
    udp_recv(sv_pcb, udp_callback, nullptr); // 注册报文处理回调
}
void UDP_Send(void *data, u16_t len) {
    //  if (is_connected) {
    struct pbuf *send_buff;
    send_buff = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_REF);
    send_buff->payload = data;
    udp_send(sv_pcb, send_buff);
    pbuf_free(send_buff);
    //  }
}
void ConnectProcess() {
    if (updata_fifo.empty())
        return;

    uint32_t in = updata_fifo.m_using;
    if (in > updata_fifo.m_out) {
        uint32_t len = std::min(uint32_t(1500 / sizeof(up_data_format)), in - updata_fifo.m_out);
        UDP_Send(
                &(updata_fifo.tail()),
                len * sizeof(up_data_format));
        updata_fifo.pop_tail(len);
    } else {
        uint32_t len = std::min(uint32_t(1500 / sizeof(up_data_format)), updata_fifo.m_size - updata_fifo.m_out);
        UDP_Send(
                &(updata_fifo.tail()),
                len * sizeof(up_data_format));
        updata_fifo.pop_tail(len);
    }
}
