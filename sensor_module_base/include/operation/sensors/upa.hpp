#pragma once

#include <cstdlib>
extern "C" {
    #include <pico/types.h>
}

#define UPA_CONVERSION_FACTOR 3.3f/(1<<12)
#define UPA_TPIN_LL 13
#define UPA_TPIN_LM 14
#define UPA_TPIN_MR 15
#define UPA_TPIN_RR 16

class UPASensor {
private:
    bool pwmActive;
    uint rx; //ADC2
    uint slices[4];
    uint pulseLength;

private:
    void gpioSetup();
    void prepRecv();
    void pulseRL(uint phaseDelay);
    void pulseLR(uint phaseDelay);

public:
    UPASensor();
    
    // transducer & receiver control
    uint16_t readRecv();

    // high-level control
    float scan();
};
