#include <iostream>

extern "C" {
    #include <pico/stdio.h>
    #include <pico/cyw43_arch.h>
    #include <hardware/adc.h>
}

#include <operation/kantoku.hpp>
#include <operation/sensors/upa.hpp>

#define BUFSIZE 1024

int main() {
    // general hardware setup
    cyw43_arch_init();
    stdio_init_all();
    adc_init();

    // create kantoku for setup, network, and pairing
    // constructor handles serial setup, network connection, pairing, and uplink creation
    Kantoku kan(ModuleType::NONE);
    sleep_ms(2000);
    
    // make sure module paired to controller
    if (!kan.attemptPair()) {
        std::cout << "FAILED TO PAIR TO CONTROLLER!" << std::endl;
        exit(1);
    }

    //UPASensor upa;

    std::cout << "REACHED END OF MAIN!" << std::endl;
    return 0;
}