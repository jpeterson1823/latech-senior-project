#include "hardware/at28hc64.hpp"
#include <iostream>
#include <iomanip>
#include <bitset>

// HAL Class for ATMEL AT28HC64B series of EEPROMs
At28hc64b::At28hc64b() {
    // Init IO pins
    gpio_init_mask(IO_MASK);
    gpio_set_dir_out_masked(IO_MASK);

    // pull down all IO pins (GPIO 0-7)
    for (uint8_t i = 0; i < 8; i++)
        gpio_pull_down(i);

    // init and set dir of shift register pins
    gpio_init(rclk);
    gpio_init(srclk);
    gpio_init(ser);
    gpio_init(we);
    gpio_init(oe);

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
    gpio_clr_mask(IO_MASK);
}

// Set IO bus GPIO pins to OUTPUT
void At28hc64b::setBusOutput() {
    if (ioDirection != EEPROM_IO_OUT) {
        //std::cout << "Setting IO bus to OUTPUT\n";
        gpio_set_dir_out_masked(IO_MASK);
        ioDirection = EEPROM_IO_OUT;
    }
}

// Set IO bus GPIO pins to INPUT
void At28hc64b::setBusInput() {
    if (ioDirection != EEPROM_IO_IN) {
        //std::cout << "Setting IO bus to INPUT\n";
        gpio_set_dir_in_masked(IO_MASK);
        ioDirection = EEPROM_IO_IN;
    }
}

// Latch current value in shift registers
void At28hc64b::shiftLatch() {
    gpio_put(rclk, true);
    sleep_us(SHIFT_REG_DELAY);
    gpio_put(rclk, false);
    sleep_us(SHIFT_REG_DELAY);
}

// Pulse shift register clock
void At28hc64b::pulseSCLK() {
    gpio_put(srclk, true);
    sleep_us(SHIFT_REG_DELAY);
    gpio_put(srclk, false);
    sleep_us(SHIFT_REG_DELAY);
}

// Shift 2 bytes of data and latch to shift registers
void At28hc64b::shiftOut(uint16_t data) {
    for (uint8_t i = 0; i < 16; i++) {
        gpio_put(ser, (data & (0x8000 >> i)) == 0 ? 0 : 1);
        pulseSCLK();
    }
    shiftLatch();
    gpio_put(ser, 0);
}

/**
 * @brief Read byte at specific address.
 * 
 * @param address Address of byte to read.
 * @return Byte stored at address.
 */
uint8_t At28hc64b::readByte(uint16_t address) {
    // set io bus to input if not already
    setBusInput();

    // shift out address and enable output
    shiftOut(address);
    sleep_us(SHIFT_REG_DELAY);
    gpio_put(oe, 1);
    sleep_us(SHIFT_REG_DELAY);

    //std::cout << std::hex << address << std::endl;

    // read data lines into var
    uint8_t data = (gpio_get_all() & IO_MASK);

    return data;
}

/**
 * @brief Read nbytes starting at provided address and load into the provided buffer.
 * 
 * @param address   Starting address.
 * @param buf       Read buffer.
 * @param nbytes    Number of bytes to read.
 */
void At28hc64b::readBytes(uint16_t address, uint8_t* buf, uint16_t nbytes) {
    // read nbytes into address, making sure that total address read doesnt overflow
    for (uint16_t i = 0; i < nbytes && i <= 0xFFFF - address; i++)
        buf[i] = readByte(address + i);
}

/**
 * @brief Read a char string starting at specific address.
 * 
 * @param address   Starting address of string.
 * @param buf       Read buffer.
 * @param buflen    Number of chars to read.
 */
void At28hc64b::readString(uint16_t address, char* sbuf, uint16_t nchars) {
    for (size_t i = 0; i < nchars; i++) {
        *sbuf = readByte(address + i);
        // if nullchar is read, end of string has been reached
        if (*sbuf == '\0')
            return;
        else
            sbuf++;
    }
}

/**
 * @brief Read a char string until a specific delimiter is found. Does not include delimiter.
 * 
 * @param address   Starting address of string.
 * @param delimiter Character in which to stop reading.
 * @param buf       Read buffer.
 * @param buflen    length of read buffer
 * @return Address of delimiter within EEPROM 
 */
uint16_t At28hc64b::readUntil(uint16_t address, char delimiter, char* sbuf, uint16_t sbsize) {
    for (size_t i = 0; i < sbsize; i++) {
        *sbuf = readByte(address + i);

        // if delimiter has been read, replace with nullchar and return current address
        if (*sbuf == delimiter) {
            *sbuf = '\0';
            return address + i;
        }
        else
            sbuf++;
    }
    // out of bounds has occurred
    return 0xFFFF;
}

/**
 * @brief Write byte to specific address.
 * 
 * @param data      Byte to write.
 * @param address   Address to write to.
 */
void At28hc64b::writeByte(uint8_t data, uint16_t address) {
    //std::cout << "writeByte(" << std::hex << std::setw(2) << std::setfill('0') << (int)data;
    //std::cout << ", " << std::hex << std::setw(2) << std::setfill('0') << (int)address;
    //std::cout << ')' << std::endl;
    // set IO bus to output if not already
    setBusOutput();

    // shift out address
    shiftOut(address);

    // write data to data bus
    gpio_put_masked(IO_MASK, (uint32_t)data);
    sleep_us(SHIFT_REG_DELAY);
    // initiate write condition: OE unset, WE pulse
    gpio_put(oe, 0);
    sleep_us(SHIFT_REG_DELAY);
    gpio_put(we, 1);
    sleep_us(SHIFT_REG_DELAY);
    gpio_put(we, 0);
    sleep_us(SHIFT_REG_DELAY);

    // unset data lines
    gpio_clr_mask(IO_MASK);
}

/**
 * @brief Write nbytes to EEPROM starting at specific address.
 * 
 * @param bytes     Byte data to write.
 * @param nbytes    Number of bytes in data.
 * @param address   Address to begin block write.
 */
void At28hc64b::writeBytes(uint8_t* bytes, uint16_t nbytes, uint16_t address) {
    for (uint16_t i = 0; i < nbytes; i++)
        writeByte(bytes[i], address+i);
}

/**
 * @brief Write a character string starting at specific address.
 * 
 * @param str       Character string to write.
 * @param address   Starting address for string write.
 */
void At28hc64b::writeString(const char* str, uint16_t address) {
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

/**
 * @brief Write float value to 4-byte block on EEPROM.
 * 
 * @param f         Float to write.
 * @param address   Address to start storing data.
 */
void At28hc64b::writeFloat(float f, uint16_t address) {
    // get raw binary of float
    uint32_t fRaw = (uint32_t)( (void*)&f );

    // write each byte in HI->LO order to eeprom
    writeByte(address,   (0xFF000000 & fRaw) >> 24);
    writeByte(address+1, (0x00FF0000 & fRaw) >> 16);
    writeByte(address+2, (0x0000FF00 & fRaw) >> 8);
    writeByte(address+3, (0x000000FF & fRaw));
}
