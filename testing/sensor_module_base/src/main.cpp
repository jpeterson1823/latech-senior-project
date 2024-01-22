#include <iostream>

extern "C" {
    #include <pico/stdlib.h>
    #include <pico/cyw43_arch.h>
    #include <lwip/tcp.h>
}

#include "networking/wifi.hpp"
#include "networking/sockets.hpp"
#include "hardware/serial_interface.hpp"
#include "hardware/parallel_eeprom.hpp"

// Various operational flags
#define WIFI_RECONNECT_TRIES 2
#define CREDS_ADDR 0x0000
#define BUFSIZE 1024*5

// LWIP Stuffs
#define TCP_SOCKET_BUFSIZE 2048
char recvBuf[TCP_SOCKET_BUFSIZE];
char connBuf[TCP_SOCKET_BUFSIZE];
err_t recvCallback(void* arg, struct tcp_pcb* pcb, struct pbuf* p, err_t err);
err_t connCallback(void* arg, struct tcp_pcb* pcb, err_t err);
std::string testHeader = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";

// Hardware Abstraction Layer struct to aid in passing variables
struct ModuleHAL {
    EEPROM prom;
};

int main() {
    stdio_init_all();
    serial_clear();

    // initialize hardware abstraction layer
    struct ModuleHAL hal;
    //rom.writeString("bingus;FizzBuzz23!", CREDS_ADDR);

    // retrieve login info from EEPROM
    char ssid[BUFSIZE];
    char pswd[BUFSIZE];
    uint16_t delimAddr = hal.prom.readUntil(CREDS_ADDR, ';', ssid, BUFSIZE);
    hal.prom.readUntil(delimAddr + 1, '\0', pswd, BUFSIZE);

    std::cout << "Retrieved Creds:\n";
    std::cout << "  SSID: " << ssid << '\n';
    std::cout << "  PSWD: " << pswd << std::endl;

    // setup and connect to wifi
    int status = Wifi::Connect(ssid, pswd);
    for (uint8_t i = 0; i < WIFI_RECONNECT_TRIES; i++) {
        if (status != CYW43_LINK_UP) {
            std::cout << "Failed to connect to network with SSID: \"" << ssid << "\"\n";
            std::cout << "Retrying (attempt " << i + 1 << '/' << WIFI_RECONNECT_TRIES << ')' << std::endl;
            status = Wifi::Connect(ssid, pswd);
        } else {
            std::cout << "Successfully connected to WiFi network." << std::endl;
            break;
        }
    }
    if (status != CYW43_LINK_UP) {
        std::cout << "Could not connect to WiFi network. Exiting..." << std::endl;
        return 1;
    }

    // copy test header to connection buffer
    strcpy(connBuf, testHeader.c_str());
    // attempt to make a get request to HTTP server
    std::cout << "Attempting tcp connection...\n";
    struct tcp_pcb* pcb = tcp_new();
    tcp_recv(pcb, recvCallback);
    ip_addr_t ip;
    IP4_ADDR(&ip, 93, 184, 216, 34);
    //IP4_ADDR(&ip, 192,168,43,49);
    cyw43_arch_lwip_begin();
    err_t err = tcp_connect(pcb, &ip, 80, connCallback);
    //err_t err = tcp_connect(pcb, &ip, 1200, connected);
    cyw43_arch_lwip_end();
    std::cout << "TCP connection attempt finished" << std::endl;
    std::cout << err << std::endl;

    // control loop
    while(true) {

    }

    return 0;
}


// lwip's tcp receive callback
err_t recvCallback(void* arg, struct tcp_pcb* pcb, struct pbuf* p, err_t err) {
    std::cout << "RECEIVED: " << p << std::endl;
    if (p != NULL) {
        std::cout << "Pbuf is not null" << std::endl;
        std::cout << "recv total " << p->tot_len << "  this buffer " << p->len << " next " << p->next << "  err " << err << std::endl;
        if (p->tot_len > 2) {
            if (!pbuf_copy_partial(p, recvBuf, p->tot_len, 0))
                std::cout << "No data in pbuf" << std::endl;
            recvBuf[p->tot_len] = 0;
            std::cout << "Buffer=" << recvBuf << std::endl;
            tcp_recved(pcb, p->tot_len);
        }
        pbuf_free(p);
    }
    else {
        std::cout << "TCP Connection Closed\n";
        err = tcp_close(pcb);
    }
    return ERR_OK;
};

// lwip's tcp connection callback
err_t connCallback(void* arg, struct tcp_pcb* pcb, err_t err) {
    std::cout << "CONNECTED" << std::endl;
    err = tcp_write(pcb, connBuf, strlen(connBuf), TCP_WRITE_FLAG_COPY);
    std::cout << "tcp_write" << std::endl;
    err = tcp_output(pcb);
    std::cout << "tcp_output" << std::endl;
    return err;
};