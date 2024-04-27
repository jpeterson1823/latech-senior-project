#pragma once

#include "networking/neosocket.hpp"
#include "networking/httpreqs.hpp"
extern "C" {
    #include <lwip/tcp.h>
}
#include <string>
#include <queue>

#define NEOSOCKET_BUFSIZE 2048
#define NEOSOCKET_RECVQ_SIZE 15

typedef struct __state_struct {
    ip_addr_t*  rhost;
    uint16_t    rport;
    uint8_t*    data;
    struct tcp_pcb* pcb;

    uint8_t* tmp;
    std::queue<std::string> recvq;

    bool initialized;
    bool connected;
    bool recving;
    bool recvd;
} state_struct;

class socket {
public:
    // keeps track of all important data
    static state_struct state;

    // initialize socket library
    static void initialize(ip_addr_t* remoteAddr, uint16_t remotePort);

    // tcp functions
    static err_t send(Http::Request req);

    // various fn's
    static void wait();
    static void pushRecv();
    static bool dataAvailable();
    static std::string popRecvq();

    // callbacks
    static err_t recvCallback(void* arg, struct tcp_pcb* pcb, struct pbuf* p, err_t err);
    static err_t connCallback(void* arg, struct tcp_pcb* pcb, err_t err);
    static err_t sentCallback(void* arg, struct tcp_pcb* pcb, err_t err);
};