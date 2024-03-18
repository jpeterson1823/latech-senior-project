#include <iostream>

extern "C" {
    #include <pico/stdio.h>
    #include <pico/cyw43_arch.h>
    #include <hardware/adc.h>
}

#include "operation/kantoku.hpp"
#include "networking/neosocket.hpp"
#include "hardware/pwmctrl.hpp"

#define BUFSIZE 1024

int main() {
    // general hardware setup
    cyw43_arch_init();
    stdio_init_all();
    adc_init();
    sleep_ms(2000);

    /*
    // create kantoku for setup, network, and pairing
    // constructor handles serial setup, network connection, pairing, and uplink creation
    Kantoku kan;
    
    // make sure module paired to controller
    if (!kan.attemptPair()) {
        std::cout << "FAILED TO PAIR TO CONTROLLER!" << std::endl;
        exit(1);
    }

    std::cout << socket::popRecvq() << std::endl;*/

    UPASensor upa;


    std::cout << "REACHED END OF MAIN!" << std::endl;
    return 0;
}