#include "networking/dhcpman.hpp"

DHCPMan::DHCPMan(std::string logPath){ 
    this->logPath = logPath;

    // fill availableIP4s with valid ip4str's (x.x.x.2-254)
    IP4 ip4("192.168.0.2");
    while (ip4.octets[3] < 255) {
        availableIP4s.push_back(ip4.toString());
        ip4.octets[3] += 1;
    }
}
