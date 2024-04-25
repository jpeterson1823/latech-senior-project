#pragma once
#include "networking/addressing.hpp"
#include <map>
#include <vector>

class DHCPLease {
friend class DHCPMan;
private:
    IP4 ip4;
    Mac mac;
public:
    DHCPLease(std::string ip4str, std::string macstr);

    IP4 getIP4();
    Mac getMac();
    std::string toString();
};

class DHCPMan {
private:
    std::string leaseFile;
    std::map<Mac*, IP4*> leases;
    std::vector<std::string> availableIP4s;

    void loadLeaseFile();
    IP4 genNewIP4();
    DHCPLease genLease(Mac mac);

public:
    DHCPMan(std::string leaseFile);
    ~DHCPMan();

    DHCPLease lease(Mac mac);
};