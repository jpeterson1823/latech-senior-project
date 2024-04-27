#pragma once
#include <pico/stdlib.h>

class Receiver {
private:
    uint gpio;
    uint16_t pulseLength;
public:
    Receiver(uint gpio_pin, uint16_t pulseLength);
    void receive(char* buf, size_t buflen);
};