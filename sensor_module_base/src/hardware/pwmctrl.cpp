#include "hardware/pwmctrl.hpp"

extern "C" {
    #include <pico/stdlib.h>
    #include <pico/time.h>
    #include <hardware/pwm.h>
    #include <hardware/adc.h>
}

UPASensor::UPASensor() {
    this->active = false;
    this->rpin = 2; // GP28
    this->pwm_pin = 16;
    this->tbita_pin = 14;
    this->tbitb_pin = 15;

    // analog pin setup
    adc_gpio_init(rpin);

    // get pwm pin slice and channel
    this->slice = pwm_gpio_to_slice_num(pwm_pin);
    uint channel = pwm_gpio_to_channel(pwm_pin);

    // set up pwm
    gpio_set_function(pwm_pin, GPIO_FUNC_PWM);
    pwm_set_phase_correct(slice, false);
    pwm_set_wrap(slice, 125000000 / 40000);
    pwm_set_chan_level(slice, channel, 125000000 / 40000 / 2);
}

// turns upa transducers on
void UPASensor::on() {
    if (!this->active) {
        this->active = true;
        pwm_set_enabled(slice, true);
    }
}

// turns upa transducers off
void UPASensor::off() {
    if (this->active) {
        this->active = false;
        pwm_set_enabled(slice, false);
    }
}

// toggles transducers on or off
void UPASensor::toggle() {
    if (!active)
        on();
    else
        off();
}

// return active state
bool UPASensor::state() {
    return this->active;
}

// pulse transducers for us microseconds
void UPASensor::pulse(uint us) {
    this->on();
    sleep_us(us);
    this->off();
}

// poll ultrasonic receiver and return read value
uint UPASensor::poll(float angle) {
    // select receiver as adc input
    adc_select_input(rpin);

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