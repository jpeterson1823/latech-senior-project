#pragma once

#include <cstdint>

extern "C" {
    #include <pico/stdlib.h>
    #include <hardware/gpio.h>
}

#define SHIFT_REG_DELAY 500
#define EEPROM_OE_MASK 0x4000
#define EEPROM_WE_MASK 0x8000
#define EEPROM_IO_IN false
#define EEPROM_IO_OUT true

class At28hc64b {
private:
    // IO Mask covers GPIO 0-7
    const uint32_t IO_MASK = 0x7F;
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
    At28hc64b();
    void writeByte(uint8_t byte, uint16_t address);
    void writeBytes(uint8_t* bytes, uint16_t nbytes, uint16_t address);
    void writeFloat(float f, uint16_t address);
    void writeString(const char* str, uint16_t address);

    uint8_t readByte(uint16_t address);
    void readBytes(uint16_t address, uint8_t* buf, uint16_t nbytes);
    void readString(uint16_t address, char* sbuf, uint16_t nchars);
    uint16_t readUntil(uint16_t address, char delimiter, char* sbuf, uint16_t sbsize);
};
