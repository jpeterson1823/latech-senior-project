extern "C" {
    #include <pico/stdio.h>
    #include <pico/cyw43_arch.h>
    #include <pico/time.h>
    #include <hardware/gpio.h>
}
#include <cstdint>
#include <iostream>
#include <iomanip>

#include "at28hc64.hpp"

int main() {
    stdio_init_all();
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_USA)) {
        std::cout << "cyw43_arch_init failed!" << std::endl;
        return -1;
    }

    At28hc64 prom;
    prom.clean();
    prom.hexdump(0x0000, 0x00FF);

    return 0;
}