#include <iostream>

extern "C" {
    #include <pico/stdlib.h>
    #include <pico/cyw43_arch.h>
    #include <lwip/tcp.h>
}

#include "networking/wifi.hpp"
#include "networking/sockets.hpp"
#include "networking/uplink.hpp"
#include "hardware/serial_interface.hpp"
#include "hardware/parallel_eeprom.hpp"

// Various operational flags
#define WIFI_RECONNECT_TRIES 2
#define CREDS_ADDR 0x0000
#define BUFSIZE 1024*5

// LWIP Stuffs
//std::string testHeader = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
std::string testHeader = "GET /pair HTTP/1.1\r\nHost: 192.168.43.238\r\n\r\n";

// Hardware Abstraction Layer struct to aid in passing variables
struct ModuleHAL {
    EEPROM prom;
};

int main() {
    stdio_init_all();
    serial_clear();

    // initialize hardware abstraction layer
    struct ModuleHAL hal;

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

    /*strcpy(connBuf, testHeader.c_str());
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
    std::cout << err << std::endl;*/

    sleep_ms(1000);

    // Create TCP Packet
    Packet packet(testHeader.c_str(), testHeader.length());

    // create IP object for example.com
    ip_addr_t ip;
    IP4_ADDR(&ip, 93, 184, 216, 34);
    //IP4_ADDR(&ip,192,168,43,238);

    // create socket object
    //SocketTCP socket(&ip, 80);
    SocketTCP socket(&ip, 2312);

    

    //ip4_addr_t linkip;
    //IP4_ADDR(&linkip,192,168,43,238);
    //Uplink link(&linkip);
    //while(true)
    //link.pair();

    while (true) {
        // attempt to make a get request to HTTP server
        std::cout << "Attempting tcp connection...\n";
        err_t err = socket.send(&packet);
        std::cout << "TCP connection attempt finished" << std::endl;
        std::cout << err << std::endl;
        sleep_ms(1000);
    }

    return 0;
}