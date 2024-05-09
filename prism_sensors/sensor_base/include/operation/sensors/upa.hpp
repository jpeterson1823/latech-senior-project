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
#define UPA_ANGLE_LIMIT 25
// change sweep resolution if you want more upa_results per sweep.
// value represents a degree step
#define UPA_SWEEP_RESOLUTION 0.1f

// 0.05s * 500,000 samples/s = 5,000 samples
// still provides range of 3.43m
#define UPA_ADC_CAPTURE_DEPTH 5000


namespace upa {
    typedef struct _upa_result {
        float angle;
        float distance;
    } result;

    typedef struct _upa_config {
        float lFovLimit;
        float rFovLimit;
        uint16_t adcGate;
    } config;

    static const config default_config = {
        lFovLimit: -45.0f,
        rFovLimit:  45.0f,
        adcGate:    50u
    };
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


    uint rx; //ADC2
    uint slices[4];
    bool pwmActive;
    uint16_t adcCaptureBuf[UPA_ADC_CAPTURE_DEPTH];

    uint dmaChannel;
    dma_channel_config dmacfg;
    upa::config config;


private:
    void adcSetup();
    void gpioSetup();
    void dmaSetup();

    void pulseCC();
    void pulseRL(uint phaseDelay);
    void pulseLR(uint phaseDelay);

    float validateAngle(float angle);
    float calcPhaseDelay(float angle);

public:
    UPASensor();

    float poll(float angle);
    void configure(upa::config config);
    // transducer & receiver control
    std::vector<upa::result> sweepScan();
    std::vector<upa::result> rangeSweep(float startAngle, float endAngle);
};