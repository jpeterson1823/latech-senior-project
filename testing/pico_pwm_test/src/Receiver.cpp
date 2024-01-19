#include "Receiver.hpp"
#include <hardware/pwm.h>

Receiver::Receiver(uint gpio_pin, uint16_t pulseLength) {
    this->pulseLength = pulseLength;
    this->gpio = gpio_pin;

    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_IN);
}

void Receiver::receive(char* buf, size_t buflen) {
    // wait for a HIGH signal for 100ms
    absolute_time_t last = get_absolute_time();
    bool waiting = false;
    for (;;) {
        if(!waiting && gpio_get(gpio)) {
            waiting = true;
            last = get_absolute_time();
        }
        else {
            if (waiting && !gpio_get(gpio)) {
                waiting = false;
                // if pulse of 100ms detected, break loop
                if (absolute_time_diff_us(last, get_absolute_time()) >= 100'000'000)
                    break;
            }
        }
    }

    // write incoming data to buffer
    uint8_t bitshift = 0;
    char* ptr = buf;
    size_t len = 0;
    char c = 0;
    for(;;) {
        // read bit from receiver
        c |= (gpio_get(gpio) ? 1 : 0) << bitshift++;
        sleep_ms(pulseLength);

        // if full char is filled, add it to buf
        if (bitshift == 8) {
            *ptr++ = c;
            len++;
            // if outta room in buffer, exit routine
            if (len == buflen)
                break;
            bitshift = 0;
            c = 0;
        }

        // if last two chars in buff are ASCII_EOT, end receive
        if (len >= 2 && buf[len-2] == 0x04 && buf[len-2] == 0x04) {
            // set null char to first EOT
            buf[len-2] = '\0';
            break;
        }
    }
}