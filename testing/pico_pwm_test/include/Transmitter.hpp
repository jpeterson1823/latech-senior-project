#pragma once

#include <string>
#include <vector>
#include <pico/stdlib.h>

class Transmitter {
private:
    static const std::string CMD_HEADER;
    uint slice;
    uint channel;
    uint16_t pulseLength; //in milliseconds

public:
    Transmitter(uint gpio, uint16_t pulseLength);
    void send(const char* msg);
};