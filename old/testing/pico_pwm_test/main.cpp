#include <iostream>
#include <cmath>
#include <cstdint>
#include <string>
extern "C" {
    #include <pico/time.h>
    #include <pico/stdlib.h>
    #include <pico/multicore.h>
    #include <hardware/pwm.h>
    #include "LiquidCrystal_I2C.h"
}

#include "Receiver.hpp"
#include "Transmitter.hpp"

#define TX_GPIO_PIN 28
#define OSC_READ_PIN 0

// GLOBAL DECLARATIONS -------------------
const unsigned long I2C_CLOCK = 100000;
const uint8_t LCD_I2C_ADDRESS = 0x27;

uint64_t osc_count = 0;

void count_callback(uint gpio, uint32_t events);
void oscilliscope();
void pwm_setup(uint& slice, uint& channel);

int main() {
    stdio_init_all();
    std::cout << "Starting Radio Test" << std::endl;
    //oscilliscope();
    //Transmitter trans(28, 200);
    Receiver recv(0, 200);

    std::cout << "WAITING" << std::endl;
    //sleep_ms(5250);
    sleep_ms(5000);

    size_t len = 1024;
    char buf[len];
    recv.receive(buf, len);
    std::cout << "RECEIVED: " << buf << std::endl;
    //trans.send("test");

    return 0;
}

void count_callback(uint gpio, uint32_t events) {
    if (gpio == OSC_READ_PIN)
        osc_count++;
}

void oscilliscope() {
    lcd_init(i2c_default, LCD_I2C_ADDRESS);
    lcd_clear();
    lcd_set_cursor(0, 0);

    gpio_set_irq_enabled_with_callback(0, GPIO_IRQ_EDGE_RISE, true, count_callback);


    gpio_init(OSC_READ_PIN);
    gpio_set_dir(OSC_READ_PIN, GPIO_IN);

    int64_t delta;
    absolute_time_t last = get_absolute_time();
    for(;;) {
        delta = absolute_time_diff_us(last, get_absolute_time());
        if (delta >= 500'000) {
            printf("%lld\n", delta);
            lcd_clear();
            lcd_set_cursor(0, 0);
            lcd_print((std::to_string(osc_count * (1'000'000/delta)) + " Hz").c_str());
            osc_count = 0;
            delta = 0;
            last = get_absolute_time();
        }
    }
}

void pwm_setup(uint& slice, uint& channel) {
    // 1kHz
    uint16_t transferFreq = 2000;
    //float div = ceil(125000000.0/(4096.0*transferFreq))/16.0;
    //uint16_t wrap = (125000000/div) / transferFreq;
    uint16_t wrap = 125000000 / transferFreq;


    gpio_set_function(TX_GPIO_PIN, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(TX_GPIO_PIN);
    channel = pwm_gpio_to_channel(TX_GPIO_PIN);

    //pwm_set_phase_correct(slice, true);
    pwm_set_phase_correct(slice, false);
    //pwm_set_clkdiv(slice, div);
    pwm_set_wrap(slice, wrap);
    pwm_set_chan_level(slice, channel, wrap/2);
}