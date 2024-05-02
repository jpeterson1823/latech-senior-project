extern "C" {
    #include <pico/stdio.h>
    #include <pico/time.h>
    #include <pico/cyw43_arch.h>
    #include <hardware/gpio.h>
}
#include <cstdint>
#include <iostream>
#include <iomanip>

#include "at28hc64.hpp"

int main() {
    stdio_init_all();
    //if (!cyw43_arch_init()) {
    //    std::cout << "cyw43_arch_init failed!" << std::endl;
    //}

    sleep_ms(5000);
    At28hc64 prom;
    prom.clean();
    prom.hexdump();

    return 0;
}