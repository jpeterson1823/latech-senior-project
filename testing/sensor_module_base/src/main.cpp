#include <iostream>

extern "C" {
    #include <pico/stdlib.h>
    #include <pico/cyw43_arch.h>
    #include <lwip/tcp.h>
}

#include "networking/wifi.hpp"
#include "serial_interface.hpp"
#include "parallel_eeprom.hpp"

#define CREDS_ADDR 0x0000
#define BUFSIZE 1024*5

char buf[BUFSIZE];
char header[] = "GET / HTTP/1.1\r\nHost: example.com\r\nAccept: text/html\r\nConnection: keep-alive\r\n\r\n";
//char header[] = "GET / HTTP/1.1\r\n\r\n";

err_t recv(void* arg, struct tcp_pcb* pcb, struct pbuf* p, err_t err);
static err_t connected(void* arg, struct tcp_pcb* pcb, err_t err);


int main() {
    // allocate all stack memory to make sure crash occurs first if out of memory
    char ssid[BUFSIZE];
    char pswd[BUFSIZE];

    // pico(w) stdio initialization
    stdio_init_all();

    // clear serial monitor
    serial_clear();

    // create necessary objects
    EEPROM rom;
    //rom.writeString("bingus;FizzBuzz23!", CREDS_ADDR);

    // retrieve login info from EEPROM
    uint16_t delimAddr = rom.readUntil(CREDS_ADDR, ';', ssid, BUFSIZE);
    rom.readUntil(delimAddr + 1, '\0', pswd, BUFSIZE);

    std::cout << "Creds:\n";
    std::cout << "  SSID: " << ssid << '\n';
    std::cout << "  PSWD: " << pswd << std::endl;

    // setup and connect to wifi
    Wifi::Connect(ssid, pswd);


    sleep_ms(1000);

    // attempt to make a get request to HTTP server
    std::cout << "Attempting tcp connection...\n";
    struct tcp_pcb* pcb = tcp_new();
    tcp_recv(pcb, recv);
    ip_addr_t ip;
    IP4_ADDR(&ip, 93, 184, 216, 34);
    //IP4_ADDR(&ip, 192,168,43,49);
    cyw43_arch_lwip_begin();
    err_t err = tcp_connect(pcb, &ip, 80, connected);
    //err_t err = tcp_connect(pcb, &ip, 1200, connected);
    cyw43_arch_lwip_end();
    std::cout << "TCP connection attempt finished" << std::endl;
    std::cout << err << std::endl;

    while(true) {
        sleep_ms(500);
    }
    return 0;
}

err_t recv(void* arg, struct tcp_pcb* pcb, struct pbuf* p, err_t err) {
    std::cout << "RECEIVED: " << p << std::endl;
    if (p != NULL) {
        std::cout << "Pbuf is not null" << std::endl;
        std::cout << "recv total " << p->tot_len << "  this buffer " << p->len << " next " << p->next << "  err " << err << std::endl;
        if (p->tot_len > 2) {
            if (!pbuf_copy_partial(p, buf, p->tot_len, 0))
                std::cout << "No data in pbuf" << std::endl;
            buf[p->tot_len] = 0;
            std::cout << "Buffer=" << buf << std::endl;
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

static err_t connected(void* arg, struct tcp_pcb* pcb, err_t err) {
    std::cout << "CONNECTED" << std::endl;
    err = tcp_write(pcb, header, strlen(header), TCP_WRITE_FLAG_COPY);
    std::cout << "tcp_write" << std::endl;
    err = tcp_output(pcb);
    std::cout << "tcp_output" << std::endl;
    return err;
}