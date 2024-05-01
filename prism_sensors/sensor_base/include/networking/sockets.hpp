#pragma once

#ifndef TCP_SOCKET_BUFSIZE
    #define TCP_SOCKET_BUFSIZE 8192
#endif

#include <vector>
#include <string>
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

// #################### IMPORTANT ######################
// CAN ONLY HAVE ONE SOCKET PER MODULE!!!!!!!
class SocketTCP {
private:
    ip_addr_t* raddr;
    uint16_t rport;

public:
    static std::vector<std::string> RecvBuf;
    static Packet* __activePacket;
    struct tcp_pcb* pcb;

public:
    SocketTCP(ip_addr_t* remoteAddr, uint16_t remotePort);

    uint8_t getState();

    ip_addr_t* getRemote();
    err_t send(Packet* packet);
    err_t sendStr(std::string& data);
    char* recv();

    static err_t __connCallback(void* arg, struct tcp_pcb* pcb, err_t err);
    static err_t __recvCallback(void* arg, struct tcp_pcb* pcb, struct pbuf* p, err_t err);
};