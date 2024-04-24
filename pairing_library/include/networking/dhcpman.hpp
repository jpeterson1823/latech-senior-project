#pragma once
#include "networking/addressing.hpp"
#include <map>
#include <vector>

typedef struct __dhcp_lease {
    IP4 ip4;
    Mac mac;
} dhcp_lease;

class DHCPMan {
private:
    std::string logPath;
    std::map<Mac, IP4> leases;
    std::vector<std::string> availableIP4s;

    IP4 genNewIP4();
    dhcp_lease genLease(Mac mac);

public:
    DHCPMan(std::string logPath);

    dhcp_lease lease(Mac mac);
};