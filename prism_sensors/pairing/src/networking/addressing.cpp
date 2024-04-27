#include "networking/addressing.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

IP4::IP4(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    octets[0] = a;
    octets[1] = b;
    octets[2] = c;
    octets[3] = d;
}

IP4::IP4(std::string ip4str) {
    uint8_t i = 0;
    std::string octet = "";
    for (char c : ip4str) {
        if (c != '.') {
            octet += c;
        }
        else {
            octets[i++] = std::stoi(octet);
            octet = "";
        }
    }
    octets[i] = std::stoi(octet);
}

std::string IP4::toString() {
    std::string s = "";
    s += std::to_string(octets[0]) + ".";
    s += std::to_string(octets[1]) + ".";
    s += std::to_string(octets[2]) + ".";
    s += std::to_string(octets[3]);

    return s;
}

void IP4::getOctets(uint8_t* buf) {
    buf[0] = octets[0];
    buf[1] = octets[1];
    buf[2] = octets[2];
    buf[3] = octets[3];
}

Mac::Mac(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f) {
    octets[0] = a;
    octets[1] = b;
    octets[2] = c;
    octets[3] = d;
    octets[4] = e;
    octets[5] = f;
}

Mac::Mac(std::string macstr) {
    uint8_t i = 0;
    std::string octet = "";
    for (char c : macstr) {
        if (c != ':') {
            octet += c;
        }
        else {
            octets[i++] = std::stoi(octet, 0, 16);
            octet = "";
        }
    }
    octets[i] = std::stoi(octet,0,16);
}

std::string Mac::toString() {
    std::stringstream s;
    for (uint8_t i = 0; i < 6; i++) {
        s << std::hex
        << std::setw(2)
        << std::setfill('0')
        << (int)octets[i];

        if (i < 5)
            s << ':';
    }
    return s.str();
}