#pragma once

#include <cstdlib>
extern "C" {
    #include <pico/types.h>
}

#define UPA_CONVERSION_FACTOR 3.3f/(1<<12)

class UPASensor {
private:
    bool active;
    uint rpin; //ADC2
    uint slice;
    uint pwm_pin;
    uint tbita_pin;
    uint tbitb_pin;

    uint poll(float angle);

public:
    UPASensor();
    void toggle();
    void on();
    void off();
    void pulse(uint us);
    bool state();

    float scan();
};
