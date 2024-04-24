#pragma once
#include <string>
#include "networking/dhcpman.hpp"

class IP4 {
friend DHCPMan;
protected:
    uint8_t octets[4];

public:
    IP4(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
    IP4(std::string ip4str);
    std::string toString();
};

class Mac {
friend DHCPMan;
protected:
    uint8_t octets[6];

public:
    Mac(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f);
    Mac(std::string macstr);
    std::string toString();
};