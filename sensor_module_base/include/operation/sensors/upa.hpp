#pragma once

#include <cstdlib>
#include <vector>
extern "C" {
    #include <pico/types.h>
    #include <hardware/dma.h>
}

#define UPA_CONVERSION_FACTOR 3.3/4096.0
#define UPA_TPIN_LL 13
#define UPA_TPIN_LM 14
#define UPA_TPIN_MR 15
#define UPA_TPIN_RR 16
#define RAD_TO_DEG  57.295779513
#define UPA_FREQ 40000
#define UPA_ADC_BASE_VOLTAGE 2
#define UPA_ANGLE_LIMIT 45
// change sweep resolution if you want more upa_results per sweep.
// value represents a degree step
#define UPA_SWEEP_RESOLUTION 0.5f

// 0.05s * 500,000 samples/s = 5,000 samples
// still provides range of 3.43m
#define UPA_ADC_CAPTURE_DEPTH 5000

struct upa_result {
    float angle;
    float distance;
};

class UPASensor {
private:
    /******** Math for constants:
     * Speed of Sound @ Sea Level:
     *  v = 343 m/s
     * Frequency of Signal:
     *  f = 40,000hz
     * Wavelength:
     *  l = v / f
     *    = 0.008575 m
     *    = 8.575 mm
     */
    const uint16_t pulseLength = 500; // 50us
    const float phaseScalar = pulseLength / 0.000343f;


    bool pwmActive;
    uint rx; //ADC2
    uint slices[4];
    uint16_t adcCaptureBuf[UPA_ADC_CAPTURE_DEPTH];

    uint dmaChannel;
    dma_channel_config dmacfg;

private:
    void adcSetup();
    void gpioSetup();
    void dmaSetup();

    void pulseCC();
    void pulseRL(uint phaseDelay);
    void pulseLR(uint phaseDelay);
    std::size_t calcUpaResultVecLen(float startAngle, float endAngle);

    float validateAngle(float angle);
    float calcPhaseDelay(float angle);

public:
    UPASensor();
    
    float poll(float angle);
    // transducer & receiver control
    std::vector<struct upa_result> sweepScan();
    std::vector<struct upa_result> rangeSweep(float startAngle, float endAngle);
};