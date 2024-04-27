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

void test() {
    while(true) {
        char c = getchar();
        std::cout << "Char Received: " << c << std::endl;
    }
}

int main() {
    // general hardware setup
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_USA)) {
        std::cout << "cyw43 failed to init!" << std::endl;
        exit(1);
    };
    cyw43_arch_enable_sta_mode();
    stdio_init_all();
    adc_init();
    gpio_init(CYW43_WL_GPIO_LED_PIN);
    sleep_ms(2000);

    std::cout << "MAIN START" << std::endl;

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

    /*SerialPacket p(PacketType::PAIR_REQ);
    uint8_t mac[] = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
    p.setPayloadSize(6);
    std::cout << p.loadIntoPayload(mac, 6) << std::endl;
    std::cout << p.toString() << std::endl;
    exit(1);*/

    Kantoku kan(ModuleType::NONE);
    
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