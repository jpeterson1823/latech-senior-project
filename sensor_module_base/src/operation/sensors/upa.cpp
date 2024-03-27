#include "operation/sensors/upa.hpp"

extern "C" {
    #include <pico/stdlib.h>
    #include <pico/time.h>
    #include <hardware/pwm.h>
    #include <hardware/adc.h>
}

// init gpio pins and set up pwm
void UPASensor::gpioSetup() {
    // analog pin setup
    adc_gpio_init(rx);   

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

UPASensor::UPASensor() {
    this->pwmActive = false;
    this->rx = 2; // GP28
    this->pulseLength = 100; // 100us

    // run GPIO setup
    gpioSetup();
}

// call this before readRecv()
void UPASensor::prepRecv() {
    // prep receiver by selecting ADC pin
    adc_select_input(this->rx);
}

uint16_t UPASensor::readRecv() {
    return adc_read();
}

void UPASensor::pulseLR(uint phaseDelay) {
    pwm_set_enabled(slices[0], true);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[1], true);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[2], true);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[3], true);

    sleep_us(pulseLength);

    pwm_set_enabled(slices[3], true);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[2], true);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[1], true);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[0], true);
}

void UPASensor::pulseRL(uint phaseDelay) {
    pwm_set_enabled(slices[3], true);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[2], true);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[1], true);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[0], true);

    sleep_us(pulseLength);

    pwm_set_enabled(slices[0], true);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[1], true);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[2], true);
    sleep_us(phaseDelay);
    pwm_set_enabled(slices[3], true);
}

/*
// poll ultrasonic receiver and return read value
uint UPASensor::poll(float angle) {
    // select receiver as adc input
    adc_select_input(rx);

    // send 1 ms pulse
    this->pulse(1000);

    // start timer
    absolute_time_t start = get_absolute_time();

    // listen for response and wait until generated voltage is above baseline
    uint16_t adc_raw;
    while (true) {
        adc_raw = adc_read();
        // if voltage goes above baseline, exit loop
        if (adc_raw * UPA_CONVERSION_FACTOR > 0.5)
            break;
    }

    // get number of microseconds taken for pulse to reach receiver
    int64_t diff = absolute_time_diff_us(start, get_absolute_time());

    return adc_get_selected_input();
}
*/