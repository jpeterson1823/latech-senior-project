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
    DHCPLease(Mac mac, IP4 ip4);

    IP4* getIP4();
    Mac* getMac();
    std::string toString();
};

class DHCPMan {
private:
    static const std::string LEASE_FILE_PATH;
    std::map<Mac*, IP4*> leases;
    std::vector<std::string> availableIP4s;

    void execLeaseLoader();
    void forkToLeaseLoader();
    void loadLeaseFile();
    IP4* genNewIP4();
    DHCPLease* genLease(Mac* mac);

public:
    DHCPMan();
    ~DHCPMan();

    DHCPLease* lease(Mac* mac);
};