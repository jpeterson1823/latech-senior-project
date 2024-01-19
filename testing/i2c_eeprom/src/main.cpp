#include <iostream>

extern "C" {
    #include "pico/stdlib.h"
}

#include "wireless/wifi.hpp"
#include "serial_interface.hpp"
#include "parallel_eeprom.hpp"

char ssid[] = "Americanproud";
char pass[] = "Jewel123!";

int main() {
    stdio_init_all();

    serial_clear();
    EEPROM rom;

    sleep_ms(1000);

    //rom.writeString("this is a massive test", 0x0000);
    //sleep_ms(1000);

    size_t len = 1024;
    char buf[len];
    rom.readString(buf, len, 0x0000);
    std::cout << "Read from EEPROM:\n" << buf << std::endl;

    // setup and connect to wifi
    //Wifi::Connect(ssid, pass);

    std::cout << std::endl;
    return 0;
}
