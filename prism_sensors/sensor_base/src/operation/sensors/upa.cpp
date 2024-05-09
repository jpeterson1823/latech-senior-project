#include "operation/sensors/upa.hpp"
#include <iostream>

extern "C" {
    #include <pico/stdlib.h>
    #include <pico/time.h>
    #include <pico/cyw43_arch.h>
    #include <hardware/pwm.h>
    #include <hardware/adc.h>
    #include <hardware/dma.h>
}
#include <cmath>

/**
 * @brief Construct a new UPASensor::UPASensor object
 */
UPASensor::UPASensor() {
    // general setup
    this->pwmActive = false;
    this->rx = 2; // GP28

    // set default configuration
    this->config = upa::default_config;

    // run GPIO and ADC setup
    gpioSetup();
    adcSetup();

    // claim dma channel and set default config
    this->dmaChannel = dma_claim_unused_channel(true);
    this->dmacfg = dma_channel_get_default_config(dmaChannel);
}

/**
 * @brief Configures DMA channel for ADC polling.
 */
void UPASensor::dmaSetup() {
    // write bytes to address
    channel_config_set_transfer_data_size(&dmacfg, DMA_SIZE_16);
    // reading from constant address
    channel_config_set_read_increment(&dmacfg, false);
    // writing to incrementing address
    channel_config_set_write_increment(&dmacfg, true);
    // pace transfers based on availability of adc samples
    channel_config_set_dreq(&dmacfg, DREQ_ADC);

    // configure DMA channel and **don't** start immediately.
    dma_channel_configure(dmaChannel, &dmacfg,
        this->adcCaptureBuf,    // memory to write to
        &adc_hw->fifo,          // address of adc fifo
        UPA_ADC_CAPTURE_DEPTH,  // number of transfers
        false                   // don't start immediately
    );
}

/**
 * @brief Set up ADC controls
 */
void UPASensor::adcSetup() {
    // analog pin setup
    adc_gpio_init(26 + rx);
    adc_select_input(rx);
    adc_fifo_setup(
        true,   // write to fifo
        true,   // enable dma data request
        1,      // dreq and irq asserted when >= 1 sample presetn
        false,  // disable error bit
        false    // shift data to 8 bits when pushing to fifo
    );
    // run full throttle
    adc_set_clkdiv(0);
}

/**
 * @brief Initialize gpio pins and set up pwm for UPASensor
 */
void UPASensor::gpioSetup() {
    gpio_init(UPA_TPIN_LL);
    gpio_init(UPA_TPIN_LM);
    gpio_init(UPA_TPIN_MR);
    gpio_init(UPA_TPIN_RR);

    // set up each pwm pin and save each slice in asc order
    this->slices[0] = pwm_gpio_to_slice_num(UPA_TPIN_LL);
    this->slices[1] = pwm_gpio_to_slice_num(UPA_TPIN_LM);
    this->slices[2] = pwm_gpio_to_slice_num(UPA_TPIN_MR);
    this->slices[3] = pwm_gpio_to_slice_num(UPA_TPIN_RR);

    // get channels temporarily to set freq
    uint channels[4];
    channels[0] = pwm_gpio_to_channel(UPA_TPIN_LL);
    channels[1] = pwm_gpio_to_channel(UPA_TPIN_LM);
    channels[2] = pwm_gpio_to_channel(UPA_TPIN_MR);
    channels[3] = pwm_gpio_to_channel(UPA_TPIN_RR);

    // set pwm freq and specifics and yadda yadda
    int pinIndex;
    for (int upaPin = UPA_TPIN_LL; upaPin <= UPA_TPIN_RR; upaPin++) {
        pinIndex = upaPin - UPA_TPIN_LL;
        gpio_set_function(upaPin, GPIO_FUNC_PWM);
        pwm_set_phase_correct(slices[pinIndex], false);
        pwm_set_wrap(slices[pinIndex], 125'000'000 / 40'000);
        pwm_set_chan_level(slices[pinIndex], channels[pinIndex], 125'000'000 / 40'000 / 2);
    }
}

/**
 * @brief Pulse transceivers from left to right, pausing inbetween for specified phase delay.
 * 
 * @param phaseDelay how long (in microseconds) to delay before firing the next transceiver
 */
void UPASensor::pulseLR(uint phaseDelay) {
    pwm_set_enabled(slices[0], true);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[1], true);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[2], true);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[3], true);

    sleep_us(pulseLength);

    pwm_set_enabled(slices[3], false);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[2], false);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[1], false);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[0], false);
}

/**
 * @brief Pulse transceivers from right to left, pausing inbetween for specified phase delay.
 * 
 * @param phaseDelay how long (in microseconds) to delay before firing the next transceiver
 */
void UPASensor::pulseRL(uint phaseDelay) {
    pwm_set_enabled(slices[3], true);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[2], true);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[1], true);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[0], true);

    sleep_us(pulseLength);

    pwm_set_enabled(slices[0], false);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[1], false);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[2], false);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[3], false);
}

/**
 * @brief Pulse transceivers at the same time (no phase delay).
 */
void UPASensor::pulseCC() {
    pwm_set_enabled(slices[3], true);
    pwm_set_enabled(slices[0], true);
    pwm_set_enabled(slices[1], true);
    pwm_set_enabled(slices[2], true);

    sleep_us(pulseLength);

    pwm_set_enabled(slices[3], false);
    pwm_set_enabled(slices[0], false);
    pwm_set_enabled(slices[1], false);
    pwm_set_enabled(slices[2], false);
}

/**
 * @brief Calculates delay for triggering transceivers to form beam at specified angle
 * 
 * @param angle     target angle
 * @return          delay time (in microseconds)
 */
float UPASensor::calcPhaseDelay(float angle) {
    angle = validateAngle(angle);
    if (angle == 0)
        return 0;
    return 87.465f*tan(angle*0.0174533f);
}

/**
 * @brief Return ceil'd/floor'd value for provided angle
 * 
 * @param angle     angle to validate
 * @return float    valid angle that's within FoV
 */
float UPASensor::validateAngle(float angle) {
    if (angle < -UPA_ANGLE_LIMIT)
        return -UPA_ANGLE_LIMIT;
    else if (angle > UPA_ANGLE_LIMIT)
        return UPA_ANGLE_LIMIT;
    else
        return angle;
}

/**
 * @brief Poll ultrasonic receiver and return distance of received ping
 * 
 * @param angle specific angle to poll
 * @return      distance measurement calculated from echo timing
 */
float UPASensor::poll(float angle) {
    // force angle into bounds
    angle = validateAngle(angle);

    // send pulse
    if (angle != 0) {
        // if negative: fire LR
        if (angle < 0)
            this->pulseLR(calcPhaseDelay(-angle));
        // otherwise, it's positive: fire RL
        else
            this->pulseRL(calcPhaseDelay(angle));
    }
    // if direction angle is 0, fire center
    else this->pulseCC();

    // select receiver as adc input
    adc_select_input(rx);
    // start dma and adc
    dmaSetup();
    dma_channel_start(dmaChannel);
    adc_run(true);

    // wait for dma to finish
    dma_channel_wait_for_finish_blocking(dmaChannel);

    // stop adc sampling and clean adc fifo
    adc_run(false);
    adc_fifo_drain();
    dma_channel_cleanup(dmaChannel);

    // get echo peaks
    uint16_t i;
    std::vector<uint16_t> peakIndicies;
    bool increasing = true;
    for (i = 1; i < UPA_ADC_CAPTURE_DEPTH; i++) {
        if (adcCaptureBuf[i-1] > adcCaptureBuf[i]) {
            if (increasing) {
                peakIndicies.push_back(i);
                increasing = false;
            }
        } else {
            increasing = true;
        }
    }

    // pad data
    i = 0;
    for (uint16_t pi : peakIndicies) {
        while(i <= pi)
            adcCaptureBuf[i++] = adcCaptureBuf[pi];
    }

    // get maximum value
    uint16_t max_i = 0;
    for (i = 0; i < UPA_ADC_CAPTURE_DEPTH; i++) {
        //std::cout << adcCaptureBuf[i] << std::endl;
        if (adcCaptureBuf[i] > adcCaptureBuf[max_i])
            max_i = i;
    }

    // if value at max index is not larger than sensitivity value, then return -1
    if (adcCaptureBuf[max_i] < config.adcGate)
        return 0;
    return (max_i/1000.0f) * 343.0f;
}

/**
 * @brief Polls a specified range within the FoV of the sensor
 * 
 * @param startAngle starting angle for range
 * @param endAngle   ending angle for range
 * @return           vector of poll results
 */
std::vector<upa::result> UPASensor::rangeSweep(float startAngle, float endAngle) {
    // make sure angles are valid and ceil/floor if not
    startAngle = validateAngle(startAngle);
    endAngle = validateAngle(endAngle);

    // create vector for sweep results
    std::vector<upa::result> sweepResult;

    // poll angles and increment by sweep resolution
    for (float angle = startAngle; angle <= endAngle; angle += UPA_SWEEP_RESOLUTION) {
        // poll and save results for specific angle
        sweepResult.push_back({
            angle:      angle,
            distance:   poll(angle)
        });
    }

    // return sweep results
    return sweepResult;
}

// poll entire FoV of sensor
std::vector<upa::result> UPASensor::sweepScan() {
    return rangeSweep(config.lFovLimit, config.rFovLimit);
}

// update configuration settings
void UPASensor::configure(upa::config config) {
    this->config.lFovLimit = config.lFovLimit;
    this->config.rFovLimit = config.rFovLimit;
    this->config.adcGate   = config.adcGate;
}
