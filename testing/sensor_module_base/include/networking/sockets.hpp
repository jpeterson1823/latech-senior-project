#pragma once

extern "C" {
    #include <lwip/tcp.h>    
}

// Simple struct to store various c-api callback fn's
typedef struct __socket_tcp_callback_struct {
    tcp_connected_fn connCallback;
    tcp_sent_fn      sentCallback;
    tcp_recv_fn      recvCallback;
    tcp_err_fn        errCallback;
    tcp_poll_fn      pollCallback;
} TCPCallbacks;


class SocketTCP {
private:
    TCPCallbacks* callbacks;
    ip_addr_t rAddr;
    uint16_t rPort;

public:
    SocketTCP(ip_addr_t remoteAddr, uint16_t remotePort);
    SocketTCP(ip_addr_t remoteAddr, uint16_t remotePort, TCPCallbacks* callbacks);

    err_t connect();
    err_t send();
    err_t close();
    void setCallbacks(TCPCallbacks* callbacks);
};