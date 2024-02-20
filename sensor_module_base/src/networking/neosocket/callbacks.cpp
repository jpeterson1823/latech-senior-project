#include "networking/neosocket.hpp"
#include <iostream>
#include <string>


// push data to recv queue
void socket::buffers::pushRecv() {
    // if room in recv queue, push tmp in string form
    if (recvq.size() < NEOSOCKET_RECVQ_SIZE)
        recvq.push_back(std::string((const char*)tmp));
    else
        std::cout << "[WARN] Failed push to recvq. Vector is full!" << std::endl;
}

err_t socket::recvCallback(void* arg, struct tcp_pcb* pcb, struct pbuf* p, err_t err) {
    std::cout << "RECEIVED: " << p << std::endl;

    if (p != NULL) {
        std::cout << "Pbuf is not null" << std::endl;
        std::cout << "recv total " << p->tot_len << "  this buffer " << p->len << " next " << p->next << "  err " << err << std::endl;

        if (p->tot_len > 2) {
            bool hasData = true;
            // copy packet data into temporary buffer
            if (!pbuf_copy_partial(p, socket::buffers::tmp, p->tot_len, 0)) {
                // unset hasData if empty
                hasData = false;
                std::cout << "No data in pbuf" << std::endl;
            }
            // add null terminator
            socket::buffers::tmp[p->tot_len] = 0;

            // if data available, push tmp to recv buff
            if (hasData)
                socket::buffers::pushRecv();

            std::cout << "Receive Buffer {\n" << socket::buffers::tmp << '}' << std::endl;
            tcp_recved(pcb, p->tot_len);
        }
        pbuf_free(p);
    } else {
        std::cout << "TCP Connection Closed\n";
        err = tcp_close(pcb);
    }

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

    return err;
}