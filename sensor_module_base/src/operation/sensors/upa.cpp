#include "operation/sensors/upa.hpp"

extern "C" {
    #include <pico/stdlib.h>
    #include <pico/time.h>
    #include <hardware/pwm.h>
    #include <hardware/adc.h>
}
#include <cmath>


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

    // run GPIO setup
    gpioSetup();
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

float UPASensor::calcPhaseDelay(float angle) {
    angle = validateAngle(angle);
    return (tan(angle * RAD_TO_DEG) - (2000 * pulseLength)) / v_sound;
}

// return ceil'd/floor'd value for provided angle
float UPASensor::validateAngle(float angle) {
    if (angle < -UPA_ANGLE_LIMIT)
        return -UPA_ANGLE_LIMIT;
    else if (angle > UPA_ANGLE_LIMIT)
        return UPA_ANGLE_LIMIT;
    else
        return angle;
}

// poll ultrasonic receiver and return distance of ping
uint64_t UPASensor::poll(float angle) {
    // force angle into bounds
    angle = validateAngle(angle);

    // select receiver as adc input
    adc_select_input(rx);

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

    // start timer
    absolute_time_t start = get_absolute_time();

    // listen for response and wait until generated voltage is above baseline
    uint16_t adc_raw;
    absolute_time_t now;
    while (true) {
        now = get_absolute_time();
        adc_raw = adc_read();

        // if voltage goes above baseline, exit loop
        if (adc_raw * UPA_CONVERSION_FACTOR > UPA_ADC_BASE_VOLTAGE)
            break;

        // if timeout reached before adc_raw goes above baseline, then nothing in range
        else if (absolute_time_diff_us(start, now) >= pollTimeout)
            break;
    }

    // return distance of echo in mm/us
    return (absolute_time_diff_us(start, now) / 2) * (v_sound / 1'000'000);
}

std::vector<struct upa_result> UPASensor::rangeSweep(float startAngle, float endAngle) {
    startAngle = validateAngle(startAngle);
    endAngle = validateAngle(endAngle);

    // todo
}

std::vector<struct upa_result> UPASensor::fullSweep() {
    return rangeSweep(-UPA_ANGLE_LIMIT, UPA_ANGLE_LIMIT);
}