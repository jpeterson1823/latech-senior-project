#pragma once

#include <cstdint>
#include <string>

const uint8_t EEPROM_ADDR = 0xA0;
const uint8_t EEPROM_READ = 0xA1;
#define EEPROM_I2C   0x50
#define EEPROM_START 0x01
#define EEPROM_STOP  0x80

void eeprom_write_byte(uint16_t addr, uint8_t byte);
uint8_t eeprom_read_addr(uint16_t addr);

void eeprom_write_str(uint16_t addr, const char* str);
void eeprom_read_str(uint16_t addr, std::string& buf);