#include "networking/sockets.hpp"
#include <iostream>
#include <cstring>
extern "C" {
    #include <pico/cyw43_arch.h>
}


// ######################
// # Packet Definitions #
// ######################

Packet::Packet() : datalen(0), bufflen(0) {}

Packet::Packet(const char* data, size_t datalen) {
    // make sure to only copy TCP_SOCKET_BUFSIZE characters to data buffer
    // minus 1 to account for possible null char of a string
    this->datalen = datalen >= TCP_SOCKET_BUFSIZE-1 ? TCP_SOCKET_BUFSIZE-1 : datalen;
    // copy data over
    for (size_t i = 0; i < this->datalen; i++)
        this->data[i] = data[i];
    this->data[this->datalen] = 0;
}

char* Packet::getDataPtr() { return this->data; }
char* Packet::getBuffPtr() { return this->buff; }


//#########################
//# SocketTCP Definitions #
//#########################

Packet* SocketTCP::__activePacket = nullptr;

SocketTCP::SocketTCP(ip_addr_t* remoteAddr, uint16_t remotePort) {
    this->raddr = remoteAddr;
    this->rport = remotePort;
    this->pcb = nullptr;
}

err_t SocketTCP::send(Packet* packet) {
    // Set active packet to provided packet
    SocketTCP::__activePacket = packet;
    
    // setup protocol control block and set recv callback
    this->pcb = tcp_new();
    tcp_recv(this->pcb, SocketTCP::__recvCallback);

    // initiate connection and send data
    cyw43_arch_lwip_begin();
    err_t err = tcp_connect(
        this->pcb,
        this->raddr,
        this->rport,
        SocketTCP::__connCallback
    );
    cyw43_arch_lwip_end();
    // return status code
    return err;
}


err_t SocketTCP::__recvCallback(void* arg, struct tcp_pcb* pcb, struct pbuf* p, err_t err) {
    std::cout << "RECEIVED: " << p << std::endl;
    if (p != NULL) {
        std::cout << "Pbuf is not null" << std::endl;
        std::cout << "recv total " << p->tot_len << "  this buffer " << p->len << " next " << p->next << "  err " << err << std::endl;
        if (p->tot_len > 2) {
            if (!pbuf_copy_partial(p, SocketTCP::__activePacket->getBuffPtr(), p->tot_len, 0))
                std::cout << "No data in pbuf" << std::endl;
            SocketTCP::__activePacket->getBuffPtr()[p->tot_len] = 0;
            std::cout << "Buffer=" << SocketTCP::__activePacket->getBuffPtr() << std::endl;
            tcp_recved(pcb, p->tot_len);
        }
        pbuf_free(p);
    }
    else {
        std::cout << "TCP Connection Closed\n";
        err = tcp_close(pcb);
    }

    return ERR_OK; 
}

err_t SocketTCP::__connCallback(void* arg, struct tcp_pcb* pcb, err_t err) {
    std::cout << "CONNECTED" << std::endl;

    err = tcp_write(
        pcb,
        SocketTCP::__activePacket->getDataPtr(),
        strlen(SocketTCP::__activePacket->getDataPtr()),
        TCP_WRITE_FLAG_COPY
    );
    std::cout << "tcp_write" << std::endl;

    err = tcp_output(pcb);
    std::cout << "tcp_output" << std::endl;

    return err;
}
