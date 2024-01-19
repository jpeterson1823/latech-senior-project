#include <iostream>
#include "wireless/wifi.hpp"


int Wifi::Setup(uint32_t country, const char* ssid, const char* pass, uint32_t auth, const char* hostname, ip_addr_t* ip, ip_addr_t* mask, ip_addr_t* gw) {
    if (cyw43_arch_init_with_country(country))
        return 1;
    cyw43_arch_enable_sta_mode();

    if (hostname != NULL)
        netif_set_hostname(netif_default, hostname);
    if (cyw43_arch_wifi_connect_async(ssid, pass, auth))
        return 2;
    
    int flashrate = 1000;
    int status = CYW43_LINK_UP + 1;

    while (status >= 0 && status != CYW43_LINK_UP) {
        int newStatus = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
        if (newStatus != status) {
            status = newStatus;
            flashrate = flashrate / (status + 1);
            std::cout << "connect status: " << status << ' ' << flashrate << std::endl;
        }
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(flashrate);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(flashrate);
    }

    if (status < 0)
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    else {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        if (ip != NULL)
            netif_set_ipaddr(netif_default, ip);
        if (mask != NULL)
            netif_set_ipaddr(netif_default, mask);
        if (gw != NULL)
            netif_set_gw(netif_default, gw);
        
        std::cout << "IP ......: " << ip4addr_ntoa(netif_ip_addr4(netif_default)) << '\n';
        std::cout << "MASK ....: " << ip4addr_ntoa(netif_ip_netmask4(netif_default)) << '\n';
        std::cout << "GATEWAY .: " << ip4addr_ntoa(netif_ip_gw4(netif_default)) << '\n';
        std::cout << "Host Name: " << netif_get_hostname(netif_default) << std::endl;
    }

    return status;
}

int Wifi::Connect(const char* ssid, const char* pass, const char* hostname) {
    return Wifi::Setup(CYW43_COUNTRY_USA, ssid, pass, CYW43_AUTH_WPA2_MIXED_PSK, hostname, NULL, NULL, NULL);
}