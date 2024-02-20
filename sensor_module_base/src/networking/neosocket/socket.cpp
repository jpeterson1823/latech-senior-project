#include "networking/neosocket.hpp"
extern "C" {
    #include <pico/cyw43_arch.h>
}
#include <iostream>


// initialize neosocket stuffs
void socket::initialize(ip_addr_t* remoteAddr, uint16_t remotePort) {
    std::cout << "NeoSocket Initializing... ";

    // check if already initialized
    if (socket::flags::initialized) {
        std::cout << "already initialized! Be Careful!!!" << std::endl;
        return;
    }

    // update socket state
    state.rhost = remoteAddr;
    state.rport = remotePort;

    // set initialized flag
    socket::flags::initialized = true;
    std::cout << "DONE!" << std::endl;
}


// send http request over tcp socket
err_t socket::send(Http::Request req) {
    // set up protocol control block and set callbacks
    state.pcb = tcp_new();
    tcp_recv(state.pcb, socket::recvCallback);

    // initiate connection and send data
    cyw43_arch_lwip_begin();
    err_t err = tcp_connect(
        state.pcb,
        state.rhost,
        state.rport,
        socket::connCallback
    );
    cyw43_arch_lwip_end();

    // return status code
    return err;
}

// wait until flag is dropped
void socket::wait() {
    while (socket::flags::recving) {
        if (socket::state.recvd)
            socket::flags::recving = false;
        else
            sleep_ms(250);
    }
}
