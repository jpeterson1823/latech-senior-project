#pragma once

#include <cstdlib>
#include <vector>
extern "C" {
    #include <pico/types.h>
}

#define UPA_CONVERSION_FACTOR 3.3f/(1<<12)
#define UPA_TPIN_LL 13
#define UPA_TPIN_LM 14
#define UPA_TPIN_MR 15
#define UPA_TPIN_RR 16
#define RAD_TO_DEG  57.295779513
#define UPA_FREQ 40000
#define UPA_ADC_BASE_VOLTAGE 0.5
#define UPA_ANGLE_LIMIT 45
// change sweep resolution if you want more upa_results per sweep.
// value represents a degree step
#define UPA_SWEEP_RESOLUTION 1

struct upa_result {
    float angle;
    float distance;
};

class UPASensor {
private:
    bool pwmActive;
    uint rx; //ADC2
    uint slices[4];

    const uint pulseLength = 250; // us
    const float v_sound = 343'000.0; // mm/s
    const float wavelength = v_sound / UPA_FREQ; // mm
    const float pulseWidth = wavelength * (pulseLength / 1000.0); // mm
    const uint64_t pollTimeout = 1'000'000; // 1M us = 1s

private:
    void gpioSetup();

    void pulseCC();
    void pulseRL(uint phaseDelay);
    void pulseLR(uint phaseDelay);

    float validateAngle(float angle);
    float calcPhaseDelay(float angle);
    uint64_t poll(float angle);

public:
    UPASensor();
    
    // transducer & receiver control
    std::vector<struct upa_result> fullSweep();
    std::vector<struct upa_result> rangeSweep(float startAngle, float endAngle);

    // high-level control
    float scan();
};