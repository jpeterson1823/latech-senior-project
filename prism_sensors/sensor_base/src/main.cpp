#include <iostream>
#include <iomanip>

extern "C" {
    #include <pico/stdio.h>
    #include <pico/cyw43_arch.h>
    #include <hardware/adc.h>
    #include <pico/multicore.h>
}

#include "operation/kantoku.hpp"
#include "operation/sensors/upa.hpp"
#include "operation/serial/session.hpp"

#define BUFSIZE 1024

int main() {
    // initialize PicoW hardware
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_USA)) {
        std::cout << "cyw43 failed to init!" << std::endl;
        exit(1);
    };
    // enable STA mode to populate netif and cyw43_state
    cyw43_arch_enable_sta_mode();
    // initialize all PicoW stdio
    stdio_init_all();
    // initialize PicoW adc hardware
    adc_init();
    // initialize built-in LED GPIO pin
    gpio_init(CYW43_WL_GPIO_LED_PIN);

    // sleep for 2 seconds to assure async hardware setup is completed
    sleep_ms(2000);
    std::cout << "MAIN START" << std::endl;

    // Start kantoku for a UPA module
    Kantoku kan(ModuleType::UPA);
    
    // if base module is not paired, attempt to pair.
    if (kan.getPairStatus() != KANTOKU_PAIRED_SUCCESSFULLY) {
        // if pairing fails, exit
        if (!kan.attemptPair()) {
            std::cout << "FAILED TO PAIR TO CONTROLLER!" << std::endl;
            exit(1);
        }
    }
    else std::cout << "PAIRED PREVIOUSLY" << std::endl;

    // create UPASensor object
    std::cout << "UPA Starting..." << std::endl;
    UPASensor upa;

    // bind upa to kantoku
    std::cout << "Attaching UPA..." << std::endl;
    kan.attachUPA(upa);

    // check for sensor config update
    std::cout << "Updating sensor config..." << std::endl;
    kan.updateSensorInfo();

    // begin main loop of sensor
    //kan.mainLoop();

    std::cout << "REACHED END OF MAIN!" << std::endl;
    return 0;
}