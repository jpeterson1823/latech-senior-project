#include "Transmitter.hpp"
#include <hardware/pwm.h>
#include <cstring>

const std::string CMD_HEADER = "_pcs";

Transmitter::Transmitter(uint gpio_pin, uint16_t pulseLength) {
    this->pulseLength = pulseLength;

    uint16_t transferFreq = 2000;
    //float div = ceil(125000000.0/(4096.0*transferFreq))/16.0;
    //uint16_t wrap = (125000000/div) / transferFreq;
    uint16_t wrap = 125000000 / transferFreq;


    gpio_set_function(gpio_pin, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(gpio_pin);
    channel = pwm_gpio_to_channel(gpio_pin);

    //pwm_set_phase_correct(slice, true);
    pwm_set_phase_correct(slice, false);
    //pwm_set_clkdiv(slice, div);
    pwm_set_wrap(slice, wrap);
    pwm_set_chan_level(slice, channel, wrap/2);
}

void Transmitter::send(const char* msg) {
    // set pwm enabled for 100ms pulse to allow for receiver to pick up signal
    pwm_set_enabled(slice, true);
    sleep_ms(100);
    pwm_set_enabled(slice, false);

    char* ptr = (char*)msg;
    uint8_t byte, i;
    while (*ptr != '\0') {
        byte = *ptr;
        // send bytes MSB first
        for(i = 7; i >= 0; i--) {
            if ((byte & (0x1 << i)) != 0)
                pwm_set_enabled(slice, true);
            else
                pwm_set_enabled(slice, false);
            sleep_ms(pulseLength);
        }
        ptr++;
    }
}