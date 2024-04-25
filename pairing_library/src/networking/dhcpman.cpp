#include "networking/dhcpman.hpp"
#include <iostream>

DHCPLease::DHCPLease(std::string ip4str, std::string macstr) : ip4(ip4str), mac(macstr) {}
IP4 DHCPLease::getIP4() { return ip4; }
Mac DHCPLease::getMac() { return mac; }
std::string DHCPLease::toString() {
    std::string s = "";
    s += mac.toString();
    s += ',';
    s += ip4.toString();
    return s;
}

DHCPMan::DHCPMan(std::string leaseFile){ 
    this->leaseFile = leaseFile;

    // 

    // fill availableIP4s with valid ip4str's (x.x.x.2-254)
    IP4 ip4("192.168.0.2");
    while (ip4.octets[3] < 255) {
        availableIP4s.push_back(ip4.toString());
        ip4.octets[3] += 1;
    }
}

DHCPMan::~DHCPMan() {
    // delete all leases from memory
    for (std::map<Mac*, IP4*>::iterator it = leases.begin(); it != leases.end(); ++it) {
        delete(it->first);
        delete(it->second);
    }
}

void DHCPMan::loadLeaseFile() {
    
}
