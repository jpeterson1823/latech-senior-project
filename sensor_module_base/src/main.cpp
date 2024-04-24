#include <iostream>
#include <iomanip>

extern "C" {
    #include <pico/stdio.h>
    #include <pico/cyw43_arch.h>
    #include <hardware/adc.h>
    #include <pico/multicore.h>
}

#include <operation/kantoku.hpp>
#include <operation/sensors/upa.hpp>

#define BUFSIZE 1024

void test() {
    while(true) {
        char c = getchar();
        std::cout << "Char Received: " << c << std::endl;
    }
}

int main() {
    // general hardware setup
    cyw43_arch_init();
    //stdio_init_all();
    adc_init();
    gpio_init(CYW43_WL_GPIO_LED_PIN);
    stdio_usb_init();
    sleep_ms(5000);

    //std::cout << "MAIN START" << std::endl;

    /*
    std::cout << "Writing to EEPROM" << std::endl;
    EEPROM rom;
    rom.writeByte(0x0000, 0x23);
    rom.writeByte(0x0001, 0x12);
    rom.writeByte(0x0002, 0x23);
    rom.writeByte(0x0003, 0x12);

    std::cout << "Values: " << std::endl;
    std::cout << "0x0000: 0x" << std::hex << std::setw(2) << std::setfill('0') << (int)rom.readByte(0x0000) << std::endl;
    std::cout << "0x0001: 0x" << std::hex << std::setw(2) << std::setfill('0') << (int)rom.readByte(0x0001) << std::endl;
    std::cout << "0x0002: 0x" << std::hex << std::setw(2) << std::setfill('0') << (int)rom.readByte(0x0002) << std::endl;
    std::cout << "0x0003: 0x" << std::hex << std::setw(2) << std::setfill('0') << (int)rom.readByte(0x0003) << std::endl;
    */

    // create kantoku for setup, network, and pairing
    // constructor handles serial setup, network connection, pairing, and uplink creation
    Kantoku kan(ModuleType::NONE);
    sleep_ms(2000);
    
    // make sure module paired to controller
    if (!kan.attemptPair()) {
        std::cout << "FAILED TO PAIR TO CONTROLLER!" << std::endl;
        exit(1);
    }

    //std::cout << "UPA Starting..." << std::endl;
    //UPASensor upa;

    //std::cout << "Polling   0.0 deg:\n"<< upa.poll(0.0f) << "mm" << std::endl;
    //std::cout << "Polling -45.0 deg:\n"<< upa.poll(-45.0f) << "mm" << std::endl;
    //std::cout << "Polling  45.0 deg:\n"<< upa.poll(45.0f) << "mm" << std::endl;

    //std::vector<struct upa_result> sweep = upa.rangeSweep(-10, 10);
    //std::cout << "Printing sweep ping results:" << std::endl;
    //for (int i = 0; i < sweep.size(); i++)
    //    std::cout << '(' << sweep[i].angle << "deg, " << sweep[i].distance << "mm)" << std::endl;


    //std::cout << "REACHED END OF MAIN!" << std::endl;
    return 0;
}