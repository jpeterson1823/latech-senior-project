#include "operation/sensors/upa.hpp"

extern "C" {
    #include <pico/stdlib.h>
    #include <pico/time.h>
    #include <hardware/pwm.h>
    #include <hardware/adc.h>
}

UPASensor::UPASensor() {
    this->pwmActive = false;
    this->rx = 2; // GP28
    this->tctrl_lbit_pin = 14;
    this->tctrl_hbit_pin = 15;
    this->pwm_pin = 16;

    // gpio setup
    gpio_init(this->tctrl_lbit_pin);
    gpio_init(this->tctrl_hbit_pin);
    gpio_init(this->pwm_pin);

    // analog pin setup
    adc_gpio_init(rx);

    // get pwm pin slice and channel
    this->slice = pwm_gpio_to_slice_num(pwm_pin);
    uint channel = pwm_gpio_to_channel(pwm_pin);

    // set up pwm
    gpio_set_function(pwm_pin, GPIO_FUNC_PWM);
    pwm_set_phase_correct(slice, false);
    pwm_set_wrap(slice, 125000000 / 40000);
    pwm_set_chan_level(slice, channel, 125000000 / 40000 / 2);
}

// starts PWM signal generation
void UPASensor::pwmOn() {
    if (!this->pwmActive) {
        this->pwmActive = true;
        pwm_set_enabled(slice, true);
    }
}

// stops PWM signal generation
void UPASensor::pwmOff() {
    if (this->pwmActive) {
        this->pwmActive = false;
        pwm_set_enabled(slice, false);
    }
}

// return active PWM state
bool UPASensor::pwmState() {
    return this->pwmActive;
}

// call this before readRecv()
void UPASensor::prepRecv() {
    // prep receiver by selecting ADC pin
    adc_select_input(this->rx);
}

uint16_t UPASensor::readRecv() {
    return adc_read();
}

void UPASensor::pulse(uint phaseDelay) {
    pwmOn();
}

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