#pragma once

#include <cstdlib>
extern "C" {
    #include <pico/types.h>
}

#define UPA_CONVERSION_FACTOR 3.3f/(1<<12)

class UPASensor {
private:
    bool pwmActive;
    uint rx; //ADC2
    uint slice;
    uint pwm_pin;

    // transducer control pins
    uint tctrl_lbit_pin;
    uint tctrl_hbit_pin;

private:
    void prepRecv();
    void pulse(uint phaseDelay);

public:
    UPASensor();
    
    // pwm control
    void pwmOn();
    void pwmOff();
    bool pwmState();

    // transducer & receiver control
    uint16_t readRecv();
    uint calcTransducerDelay(float angle);

    // high-level control
    float scan();
};
