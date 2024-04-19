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
    gpio_init(CYW43_WL_GPIO_LED_PIN);
    sleep_ms(5000);

    std::cout << "MAIN START" << std::endl;

    /*
    // create kantoku for setup, network, and pairing
    // constructor handles serial setup, network connection, pairing, and uplink creation
    Kantoku kan(ModuleType::NONE);
    sleep_ms(2000);
    
    // make sure module paired to controller
    if (!kan.attemptPair()) {
        std::cout << "FAILED TO PAIR TO CONTROLLER!" << std::endl;
        exit(1);
    }
    */

    std::cout << "UPA Starting..." << std::endl;
    UPASensor upa;

    //float angle = 0.0f;
    //std::cout << "Polling " << angle << "deg:\n"<< upa.poll(angle) << "mm" << std::endl;

    std::vector<struct upa_result> sweep = upa.rangeSweep(-45, 45);

    std::cout << "Printing sweep ping results:" << std::endl;
    for (int i = 0; i < sweep.size(); i++)
        std::cout << '(' << sweep[i].angle << "deg, " << sweep[i].distance << "mm)" << std::endl;

    std::cout << "REACHED END OF MAIN!" << std::endl;
    return 0;
}