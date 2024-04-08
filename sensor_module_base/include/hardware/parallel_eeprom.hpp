#pragma once

#include <cstdint>

extern "C" {
    #include <pico/stdlib.h>
    #include <hardware/gpio.h>
}

#define SHIFT_REG_DELAY 10
#define EEPROM_OE_MASK 0x4000
#define EEPROM_WE_MASK 0x8000
#define EEPROM_IO_IN false
#define EEPROM_IO_OUT true

class EEPROM {
private:
    // IO Mask covers GPIO 0-7
    const uint32_t IO_MASK = 0b00000000'00000000'00000000'01111111;
    // old io mask below
    //const uint32_t IO_MASK = 0b00000000'00000000'00111111'11000000;
    const uint8_t rclk  = 8;
    const uint8_t srclk = 9;
    const uint8_t ser   = 10;
    const uint8_t oe    = 11;
    const uint8_t we    = 12;
    bool ioDirection    = false;

private:
    void pulseSCLK();
    void shiftLatch();
    void shiftOut(uint16_t data);

    void setBusOutput();
    void setBusInput();

public:
    EEPROM();
    void writeByte(uint8_t data, uint16_t address);
    void writeString(const char* str, uint16_t address);
    uint8_t readByte(uint16_t address);
    void readString(uint16_t address, char* buf, size_t buflen);
    uint16_t readUntil(uint16_t address, char delimiter, char* buf, size_t buflen);
};
