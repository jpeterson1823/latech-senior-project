#include "networking/neosocket.hpp"
extern "C" {
    #include <pico/cyw43_arch.h>
}
#include <iostream>

// create state struct and set everything to init values
state_struct socket::state = {
    rhost:     nullptr,
    rport:     0,
    data:      nullptr,
    pcb:       nullptr,

    tmp: (uint8_t*)calloc(NEOSOCKET_BUFSIZE, sizeof(uint8_t)),
    recvq: { },

    initialized: false,
    connected:   false,
    recving:     false,
    recvd:       false
};

// initialize neosocket stuffs
void socket::initialize(ip_addr_t* remoteAddr, uint16_t remotePort) {
    std::cout << "NeoSocket Initializing... ";

    // check if already initialized
    if (state.initialized) {
        std::cout << "already initialized! Be Careful!!!" << std::endl;
        return;
    }

    // update socket state
    state.rhost = remoteAddr;
    state.rport = remotePort;

    // set initialized flag
    state.initialized = true;
    std::cout << "DONE!" << std::endl;
}


// send http request over tcp socket
err_t socket::send(Http::Request req) {
    std::string buf;
    req.genString(buf);

    std::cout << "Sending HTTP Request:\n\n" << buf << "\n" << std::endl;

    state.data = (uint8_t*)buf.c_str();

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
    while (!state.recvd) {
        sleep_ms(250);
    }
}


// push data to recv queue
void socket::pushRecv() {
    // if room in recv queue, push tmp in string form
    if (state.recvq.size() < NEOSOCKET_RECVQ_SIZE)
        state.recvq.push(std::string((const char*)state.tmp));
    else
        std::cout << "[WARN] Failed push to recvq. Vector is full!" << std::endl;
}

bool socket::dataAvailable() {
    return state.recvq.size() > 0;
}

std::string socket::popRecvq() {
    std::string temp = state.recvq.front();
    state.recvq.pop();
    return temp;
}

// callbacks
err_t socket::recvCallback(void* arg, struct tcp_pcb* pcb, struct pbuf* p, err_t err) {
    std::cout << "RECEIVED: " << p << std::endl;

    if (p != NULL) {
        std::cout << "Pbuf is not null" << std::endl;
        std::cout << "recv total " << p->tot_len << "  this buffer " << p->len << " next " << p->next << "  err " << err << std::endl;

        if (p->tot_len > 2) {
            bool hasData = true;
            // copy packet data into temporary buffer
            if (!pbuf_copy_partial(p, state.tmp, p->tot_len, 0)) {
                // unset hasData if empty
                hasData = false;
                std::cout << "No data in pbuf" << std::endl;
            }
            // add null terminator
            state.tmp[p->tot_len] = 0;

            // if data available, push tmp to recv buff
            if (hasData)
                socket::pushRecv();

            std::cout << "Receive Buffer {\n" << state.tmp << '}' << std::endl;
            tcp_recved(pcb, p->tot_len);
        }
        pbuf_free(p);
    } else {
        std::cout << "TCP Connection Closed\n";
        err = tcp_close(pcb);
    }

    state.recving = false;
    state.recvd = true;

    return ERR_OK; 
}

err_t socket::connCallback(void* arg, struct tcp_pcb* pcb, err_t err) {
    std::cout << "CONNECTED" << std::endl;

    err = tcp_write(
        pcb,
        state.data,
        strlen((const char*)state.data),
        TCP_WRITE_FLAG_COPY
    );
    std::cout << "tcp_write" << std::endl;

    err = tcp_output(pcb);
    std::cout << "tcp_output" << std::endl;

    state.recving = true;

    return err;
}