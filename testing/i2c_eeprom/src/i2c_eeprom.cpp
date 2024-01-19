#include "i2c_eeprom.hpp"
#include <iostream>
extern "C" {
    #include <hardware/i2c.h>
}

void eeprom_write_byte(uint16_t addr, uint8_t data) {
    //uint8_t high = addr >> 8;
    //uint8_t low = addr & 0x00FF;
    uint8_t packet[] = {
        addr >> 8,
        addr & 0x00FF,
        data
    };
    i2c_write_blocking(i2c_default, EEPROM_I2C, packet, 3, false);
    sleep_ms(5);
}

uint8_t eeprom_read_addr(uint16_t addr) {
    //uint8_t high = addr >> 8;
    //uint8_t low = addr & 0x00FF;

    uint8_t packet[] = {
        addr >> 8,
        addr & 0x00FF,
    };

    uint8_t recv = 0x10;
    i2c_write_blocking(i2c_default, EEPROM_I2C, packet, 2, true);
    sleep_ms(5);
    i2c_read_blocking(i2c_default, EEPROM_I2C, &recv, 1, false);
    sleep_ms(5);

    //std::cout << "addr: 0x" << std::hex << (addr >> 8) << (addr & 0x00FF) << std::dec << std::endl;
    //std::cout << "recv: 0x" << std::hex << (int)recv << std::dec << std::endl;

    return recv;
}

void eeprom_write_str(uint16_t addr, const char* str) {
    char* ptr = (char*)str;
    while (*ptr != '\0') {
        eeprom_write_byte(addr++, *ptr);
        ptr++;
    }
    eeprom_write_byte(addr, '\0');
}

void eeprom_read_str(uint16_t addr, std::string& buf) {
    char c;
    for(;;) {
        c = eeprom_read_addr(addr++);
        buf += c;
        if (c == '\0')
            break;
    }
}