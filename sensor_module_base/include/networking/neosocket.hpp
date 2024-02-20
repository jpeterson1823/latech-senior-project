#pragma once

#include "networking/httpreqs.hpp"
extern "C" {
    #include <lwip/tcp.h>
}
#include <vector>

#define NEOSOCKET_BUFSIZE 2048
#define NEOSOCKET_RECVQ_SIZE 15

namespace socket {
    namespace buffers {
        static uint8_t* tmp;
        static std::vector<std::string> recvq;

        static void pushRecv();
    };

    namespace flags {
        static bool initialized;
        static bool recving;
    };

    typedef struct state_struct{
        ip_addr_t*  rhost;
        uint16_t    rport;
        uint8_t*    data;
        struct tcp_pcb* pcb;
        bool connected;
        bool recvd;
    };
    

    // keeps track of all important data
    static state_struct state;

    // initialize socket library
    static void initialize(ip_addr_t* remoteAddr, uint16_t remotePort);

    // tcp functions
    err_t send(Http::Request req);

    // wait until timeout reached
    void wait();

    // callbacks
    static err_t recvCallback(void* arg, struct tcp_pcb* pcb, struct pbuf* p, err_t err);
    static err_t connCallback(void* arg, struct tcp_pcb* pcb, err_t err);
    static err_t sentCallback(void* arg, struct tcp_pcb* pcb, err_t err);
};