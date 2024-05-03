#include <iostream>
#include <sstream>
#include <iomanip>
#include "networking/wifi.hpp"

extern "C" {
    #include <pico/stdlib.h>
    #include <pico/cyw43_arch.h>
}

bool Wifi::SetupComplete = false;

int Wifi::Setup(uint32_t country, const char* ssid, const char* pass, uint32_t auth, const char* hostname, ip_addr_t* ip, ip_addr_t* mask, ip_addr_t* gw) {
    if (!SetupComplete) {
        //if (cyw43_arch_init_with_country(country))
        //    return 1;
        //cyw43_arch_enable_sta_mode();
        SetupComplete = true;
    }

    if (hostname != NULL)
        netif_set_hostname(netif_default, hostname);
    if (ip != NULL) {
        netif_set_addr(netif_default, ip, mask, gw);
    }
    if (cyw43_arch_wifi_connect_async(ssid, pass, auth))
        return 2;
    
    int flashrate = 1000;
    int status = CYW43_LINK_UP + 1;

    while (status >= 0 && status != CYW43_LINK_UP) {
        int newStatus = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
        if (newStatus != status) {
            status = newStatus;
            flashrate = flashrate / (abs(status) + 1);
            std::cout << "connect status: " << status << ' ' << flashrate << std::endl;
        }
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(flashrate);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(flashrate);
    }

    // check if connection has failed. if so, determine why
    if (status < 0) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        switch (status) {
            case CYW43_LINK_FAIL:
                std::cout << "Connection failed." << std::endl;
                break;
            case CYW43_LINK_NONET:
                std::cout << "No matching SSID found." << std::endl;
                break;
            case CYW43_LINK_BADAUTH:
                std::cout << "Authentication failure." << std::endl;
                break;
            default:
                std::cout << "Unknown error code: " << status << std::endl;
                break;
        }
    }

    // otherwise, display network info over serial
    else {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        //if (ip != NULL)
        //    netif_set_ipaddr(netif_default, ip);
        //if (mask != NULL)
        //    netif_set_ipaddr(netif_default, mask);
        //if (gw != NULL)
        //    netif_set_gw(netif_default, gw);
        
        std::cout << "IP ......: " << ip4addr_ntoa(netif_ip_addr4(netif_default)) << '\n';
        std::cout << "MASK ....: " << ip4addr_ntoa(netif_ip_netmask4(netif_default)) << '\n';
        std::cout << "GATEWAY .: " << ip4addr_ntoa(netif_ip_gw4(netif_default)) << '\n';
        std::cout << "Host Name: " << netif_get_hostname(netif_default) << std::endl;
    }

    // return connection status
    return status;
}

int Wifi::Connect(const char* ssid, const char* pass, ip_addr_t* ip, ip_addr_t* mask, ip_addr_t* gw, const char* hostname) {
    return Wifi::Setup(CYW43_COUNTRY_USA, ssid, pass, CYW43_AUTH_WPA2_MIXED_PSK, hostname, ip, mask, gw);
}

void Wifi::GetMacString(std::string& buf) {
    // get mac addr
    uint8_t mac[6];
    cyw43_wifi_get_mac(&cyw43_state, CYW43_ITF_STA, mac);

    std::stringstream macStream;

    // convert mac addr to str and store in data
    macStream << std::hex << std::setfill('0') << std::setw(2) << (int)mac[0];
    macStream << ':';
    macStream << std::hex << std::setfill('0') << std::setw(2) << (int)mac[1];
    macStream << ':';
    macStream << std::hex << std::setfill('0') << std::setw(2) << (int)mac[2];
    macStream << ':';
    macStream << std::hex << std::setfill('0') << std::setw(2) << (int)mac[3];
    macStream << ':';
    macStream << std::hex << std::setfill('0') << std::setw(2) << (int)mac[4];
    macStream << ':';
    macStream << std::hex << std::setfill('0') << std::setw(2) << (int)mac[5];

    // convert stream to string
    buf = macStream.str();
}
