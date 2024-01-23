#pragma once

#ifndef TCP_SOCKET_BUFSIZE
    #define TCP_SOCKET_BUFSIZE 8192
#endif

extern "C" {
    #include <lwip/tcp.h>    
}

class Packet {
private:
    char data[TCP_SOCKET_BUFSIZE];
    char buff[TCP_SOCKET_BUFSIZE];
    size_t datalen;
    size_t bufflen;
public:
    Packet();
    Packet(const char* data, size_t datalen);

    char* getDataPtr();
    char* getBuffPtr();
};

class SocketTCP {
private:
    ip_addr_t* raddr;
    uint16_t rport;
public:
    static Packet* __activePacket;
    struct tcp_pcb* pcb;

public:
    SocketTCP(ip_addr_t* remoteAddr, uint16_t remotePort);

    err_t send(Packet* packet);

    static err_t __connCallback(void* arg, struct tcp_pcb* pcb, err_t err);
    static err_t __recvCallback(void* arg, struct tcp_pcb* pcb, struct pbuf* p, err_t err);
};