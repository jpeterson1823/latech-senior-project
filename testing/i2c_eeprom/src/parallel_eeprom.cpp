#include "parallel_eeprom.hpp"
#include <iostream>
#include <bitset>

EEPROM::EEPROM() {
    // Init IO pins
    gpio_init_mask(IO_MASK);

    // init and set dir of shift register pins
    gpio_init(rclk);
    gpio_init(srclk);
    gpio_init(ser);
    gpio_init(we);
    gpio_init(oe);

    gpio_set_dir_out_masked(IO_MASK);

    gpio_set_dir(rclk,  GPIO_OUT);
    gpio_set_dir(srclk, GPIO_OUT);
    gpio_set_dir(ser,   GPIO_OUT);

    gpio_set_dir(we, GPIO_OUT);
    gpio_set_dir(oe, GPIO_OUT);

    gpio_put(rclk, false);
    gpio_put(ser, false);
    gpio_put(srclk, false);

    gpio_put(we, 0);
    gpio_put(oe, 1);

    ioDirection = true;
    gpio_put_masked(IO_MASK, 0x0000);
}

void EEPROM::shiftLatch() {
    gpio_put(rclk, true);
    sleep_us(SHIFT_REG_DELAY);
    gpio_put(rclk, false);
    sleep_us(SHIFT_REG_DELAY);
}

void EEPROM::pulseSCLK() {
    gpio_put(srclk, true);
    sleep_us(SHIFT_REG_DELAY);
    gpio_put(srclk, false);
    sleep_us(SHIFT_REG_DELAY);
}

void EEPROM::shiftOut(uint16_t data) {
    gpio_put(ser, (data & 0x8000) == 0 ? 0 : 1);
    pulseSCLK();
    gpio_put(ser, (data & 0x4000) == 0 ? 0 : 1);
    pulseSCLK();
    gpio_put(ser, (data & 0x2000) == 0 ? 0 : 1);
    pulseSCLK();
    gpio_put(ser, (data & 0x1000) == 0 ? 0 : 1);
    pulseSCLK();
    gpio_put(ser, (data & 0x0800) == 0 ? 0 : 1);
    pulseSCLK();
    gpio_put(ser, (data & 0x0400) == 0 ? 0 : 1);
    pulseSCLK();
    gpio_put(ser, (data & 0x0200) == 0 ? 0 : 1);
    pulseSCLK();
    gpio_put(ser, (data & 0x0100) == 0 ? 0 : 1);
    pulseSCLK();
    gpio_put(ser, (data & 0x0080) == 0 ? 0 : 1);
    pulseSCLK();
    gpio_put(ser, (data & 0x0040) == 0 ? 0 : 1);
    pulseSCLK();
    gpio_put(ser, (data & 0x0020) == 0 ? 0 : 1);
    pulseSCLK();
    gpio_put(ser, (data & 0x0010) == 0 ? 0 : 1);
    pulseSCLK();
    gpio_put(ser, (data & 0x0008) == 0 ? 0 : 1);
    pulseSCLK();
    gpio_put(ser, (data & 0x0004) == 0 ? 0 : 1);
    pulseSCLK();
    gpio_put(ser, (data & 0x0002) == 0 ? 0 : 1);
    pulseSCLK();
    gpio_put(ser, (data & 0x0001) == 0 ? 0 : 1);
    pulseSCLK();
    gpio_put(ser, 0);
    shiftLatch();
}


uint8_t EEPROM::readByte(uint16_t address) {
    // set IO lines to read
    if (ioDirection) {
        std::cout << "Setting IO bus to INPUT\n";
        gpio_set_dir_in_masked(IO_MASK);
        ioDirection = false;
    }

    // shift out address and enable output
    shiftOut(address);
    gpio_put(oe, 1);
    sleep_ms(1);

    std::cout << std::hex << address << std::endl;

    // read data lines into var
    uint8_t data = (gpio_get_all() & IO_MASK) >> 6;

    return data;
}

void EEPROM::readString(char* buf, size_t buflen, uint16_t address) {
    for (size_t i = 0; i < buflen; i++) {
        *buf = readByte(address + i);
        std::cout << *buf << ", " << i << std::endl;
        sleep_ms(1000);
        if (*buf == '\0')
            break;
        else
            buf++;
    }
}

void EEPROM::writeByte(uint8_t data, uint16_t address) {
    // set IO bus to output
    if (!ioDirection) {
        std::cout << "Setting IO bus to OUTPUT\n";
        gpio_set_dir_out_masked(IO_MASK);
        ioDirection = true;
    }

    // shift out address
    shiftOut(address);
    sleep_ms(1000);

    // write data to data bus
    gpio_put_masked(IO_MASK, (uint32_t)data << 6);
    //std::cout << "IO_MASK: " << std::bitset<32>(IO_MASK) << '\n';
    //std::cout << "DATA   : " << std::bitset<32>((uint32_t)data << 6) << std::endl;
    sleep_us(10);
    // initiate write condition: OE unset, WE pulse
    gpio_put(oe, 0);
    gpio_put(we, 1);
    sleep_ms(10);
    gpio_put(we, 0);

    // unset data lines
    gpio_put_masked(IO_MASK, 0x0000);
}

void EEPROM::writeString(const char* str, uint16_t address) {
    for(uint16_t i = 0;;i++) {
        writeByte(*(str+i), address+i);
        std::cout << *(str+i) << " @ ";
        std::cout.width(4);
        std::cout.fill('0');
        std::cout << std::hex << address + i << std::endl;
        if (*(str+i) == '\0')
            break;
    }
}