#include "networking/uplink.hpp"
#include <string>
#include <iostream>
#include <ctime>

extern "C" {
    #include <cyw43.h>
}

#define UPLINK_TIMEOUT_S 10

// constructor
Uplink::Uplink(ip4_addr_t* rhost) : link(rhost, UPLINK_INIT_PORT) {
    // get the mac addr of sensor module
    cyw43_wifi_get_mac(&cyw43_state, CYW43_ITF_STA, mac);
}


// generates HTTP Post pairing request
std::string Uplink::genPairRequest(std::string& data) {
    // get string form of rhost
    std::string rhost_addr_str;
    ipaddr_aton(rhost_addr_str.data(), this->link.getRemote());

    // create post string
    std::string form;
    form += "POST /pair HTTP/1.1\r\n";
    form += "Host: " + rhost_addr_str + "\r\n";
    form += "\r\n";
    form += data;
    form += "\r\n";

    // return string
    return form;
}

// creates string version of mac addr
std::string Uplink::getMacStr() {
    std::string addr;
    // loop through each octet and add to string sep by '-'
    for (uint8_t i = 0; i < 5; i++)
        addr += std::to_string(mac[i]) + '-';
    addr += std::to_string(mac[5]);
    return addr;
}

// pair sensor module to controller
uint8_t Uplink::pair() {
    std::cerr << "Starting pair protocol...\n";

    // creat http post req
    std::string data = ";;REQ:PAIR;PARAM:" + getMacStr() + UPLINK_EOT;
    std::string pairReq = genPairRequest(data);

    // send data to uplink
    link.sendStr(pairReq);
    std::cerr << "  Pair POST sent. Awaiting response...\n";
    
    // wait for 10 second response
    std::time_t t0 = std::time(nullptr);
    std::time_t t1;
    while (SocketTCP::RecvBuf.size() == 0) {
        t1 = std::time(nullptr);
        if (t1 - t0 >= 50) {
            std::cout << "  Pair request timed out." << std::endl;
            break;
        }
    }

    std::cout << "Response: " << SocketTCP::RecvBuf[0] << std::endl;
}