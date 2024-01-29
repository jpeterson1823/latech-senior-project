#include "networking/uplink.hpp"
#include <string>

extern "C" {
    #include <cyw43.h>
}

Uplink::Uplink(ip4_addr_t* rhost)
  : link(rhost, UPLINK_INIT_PORT) { }

uint8_t Uplink::pair() {
    cyw43_wifi_get_mac(&cyw43_state, CYW43_ITF_STA, mac);
    std::string req = "";
    req += ";;REQ.PAIR;PARAM.[";
    for (uint8_t i = 0; i < 5; i++)
        req += std::to_string(mac[0]) + ":";
    req += std::to_string(mac[5]) + "];" + UPLINK_EOT;
    Packet p(req.c_str(), req.length());
    link.send(&p);
}