#include "hardware/parallel_eeprom.hpp"
#include <iostream>
#include <bitset>

EEPROM::EEPROM() {
    // Init IO pins
    gpio_init_mask(IO_MASK);

    // pull down all IO pins (GPIO 9-15)
    for (uint8_t i = 9; i < 16; i++)
        gpio_pull_down(i);

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

    gpio_pull_down(rclk);
    gpio_pull_down(srclk);
    gpio_pull_down(ser);

    gpio_set_dir(we, GPIO_OUT);
    gpio_set_dir(oe, GPIO_OUT);
    
    gpio_pull_down(we);
    gpio_pull_down(oe);

    gpio_put(rclk, false);
    gpio_put(ser, false);
    gpio_put(srclk, false);

    gpio_put(we, 0);
    gpio_put(oe, 1);

    ioDirection = true;
    gpio_put_masked(IO_MASK, 0x0000);
}

void EEPROM::setBusOutput() {
    if (ioDirection != EEPROM_IO_OUT) {
        std::cout << "Setting IO bus to OUTPUT\n";
        gpio_set_dir_out_masked(IO_MASK);
        ioDirection = EEPROM_IO_OUT;
    }
}

void EEPROM::setBusInput() {
    if (ioDirection != EEPROM_IO_IN) {
        std::cout << "Setting IO bus to INPUT\n";
        gpio_set_dir_in_masked(IO_MASK);
        ioDirection = EEPROM_IO_IN;
    }
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
    for (uint8_t i = 0; i < 16; i++) {
        gpio_put(ser, (data & (0x8000 >> i)) == 0 ? 0 : 1);
        pulseSCLK();
    }
    shiftLatch();
    gpio_put(ser, 0);
}


uint8_t EEPROM::readByte(uint16_t address) {
    // set io bus to input if not already
    setBusInput();

    // shift out address and enable output
    shiftOut(address);
    gpio_put(oe, 1);
    sleep_us(SHIFT_REG_DELAY);

    //std::cout << std::hex << address << std::endl;

    // read data lines into var
    uint8_t data = (gpio_get_all() & IO_MASK) >> 6;

    return data;
}

void EEPROM::readString(uint16_t address, char* buf, size_t buflen) {
    for (size_t i = 0; i < buflen; i++) {
        *buf = readByte(address + i);
        // if nullchar is read, end of string has been reached
        if (*buf == '\0')
            return;
        else
            buf++;
    }
}

uint16_t EEPROM::readUntil(uint16_t address, char delimiter, char* buf, size_t buflen) {
    for (size_t i = 0; i < buflen; i++) {
        *buf = readByte(address + i);

        // if delimiter has been read, replace with nullchar and return current address
        if (*buf == delimiter) {
            *buf = '\0';
            return address + i;
        }
        else
            buf++;
    }
    // out of bounds has occurred
    return 0xFFFF;
}

void EEPROM::writeByte(uint8_t data, uint16_t address) {
    // set IO bus to output if not already
    setBusOutput();

    // shift out address
    shiftOut(address);

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
    sleep_ms(10);

    // unset data lines
    gpio_put_masked(IO_MASK, 0x0000);
}

void EEPROM::writeString(const char* str, uint16_t address) {
    for(uint16_t i = 0;;i++) {
        writeByte(*(str+i), address+i);
        //std::cout << *(str+i) << " @ ";
        //std::cout.width(4);
        //std::cout.fill('0');
        //std::cout << std::hex << address + i << std::endl;
        if (*(str+i) == '\0')
            break;
    }
}