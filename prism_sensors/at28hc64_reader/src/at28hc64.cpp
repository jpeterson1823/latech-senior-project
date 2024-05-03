#include "at28hc64.hpp"

extern "C" {
    #include <pico/stdio.h>
    #include <pico/time.h>
    #include <pico/cyw43_arch.h>
    #include <hardware/gpio.h>
}
#include <iostream>
#include <iomanip>
#include <bitset>
#include <cstdint>

At28hc64::At28hc64() { gpioSetup(); };
void At28hc64::oeSetHi() {
    if (oeState)
        return;
    gpio_put(OE, true);
    oeState = true;
}
void At28hc64::oeSetLo() {
    if (!oeState)
        return;
    gpio_put(OE, false);
    oeState = false;
}
void At28hc64::weSetHi() {
    if (weState)
        return;
    gpio_put(WE, true);
    weState = true;
}
void At28hc64::weSetLo() {
    if (!weState)
        return;
    gpio_put(WE, false);
    weState = false;
}
void At28hc64::ceSetHi() {
    if (ceState)
        return;
    gpio_put(CE, true);
    ceState = true;
}
void At28hc64::ceSetLo() {
    if (!ceState)
        return;
    gpio_put(CE, false);
    ceState = false;
}

void At28hc64::gpioSetup() {
    // init gpio pins
    gpio_init_mask(ADDR_BUS_MASK | DATA_BUS_MASK);
    gpio_init(WE);
    gpio_init(OE);
    gpio_init(CE);

    // set io directions
    gpio_set_dir_masked(ADDR_BUS_MASK | DATA_BUS_MASK, ADDR_BUS_MASK);
    gpio_set_dir(WE, GPIO_OUT);
    gpio_set_dir(OE, GPIO_OUT);
    gpio_set_dir(CE, GPIO_OUT);

    // clear busses
    gpio_clr_mask(ADDR_BUS_MASK);

    ceSetLo();
    oeSetLo();
    weSetHi();
    ceState = false;
    oeState = false;
    weState = true;
}

void At28hc64::dbusOut() {
    if (dbusState == DBUS_OUT)
        return;
    gpio_set_dir_out_masked(DATA_BUS_MASK);
    dbusState = DBUS_OUT;
}

void At28hc64::dbusIn() {
    if (dbusState == DBUS_IN)
        return;
    gpio_set_dir_in_masked(DATA_BUS_MASK);
    dbusState = DBUS_IN;
}

uint32_t At28hc64::shiftAddr(uint16_t addr) {
    return (uint32_t)(addr) << 8;
}

uint8_t At28hc64::readByte(uint16_t addr) {
    // set databus in if not already so
    dbusIn();

    // put address on address bus
    gpio_put_masked(ADDR_BUS_MASK, shiftAddr(addr));

    // enable output
    oeSetLo();

    // read databus and return it
    return (uint8_t)(gpio_get_all() & DATA_BUS_MASK);
}
void At28hc64::writeByte(uint8_t byte, uint16_t addr) {
    // set dbus out if not already soe
    dbusOut();
    // put address and data on busses
    gpio_put_masked(ADDR_BUS_MASK | DATA_BUS_MASK, shiftAddr(addr) | ((uint32_t)byte));

    // disable output
    oeSetHi();

    // pulse WE to trigger write
    weSetLo();
    weSetHi();
}

std::string At28hc64::byteHex(uint8_t byte) {
    std::stringstream s;
    s << std::hex << std::setw(2) << std::setfill('0') << (uint16_t)byte;
    return s.str();
}

void At28hc64::printByte(uint8_t byte) {
    std::cout << byteHex(byte);
}

void At28hc64::hexdump() {
    hexdump(0x0000, 0xFFFF);
}

void At28hc64::hexdump(uint16_t startAddr, uint16_t endAddr) {
    std::cout.flush();
    std::cout << std::endl;
    std::cout << "B# 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "   ";
    for (uint16_t addr = startAddr; addr <= endAddr; addr++) {
        if (addr % 16 == 0 && addr != 0x0000) {
            std::cout << "\n   ";
            printByte(readByte(addr));
            std::cout << ' ';
        }
        else {
            printByte(readByte(addr));
            std::cout << ' ';
        }
    }
    std::cout << std::endl;
}

void At28hc64::decypher() {
    // extract ssid and pswd
    std::string ssid, pswd;
    std::string* current = &ssid;
    uint8_t byte;
    for (uint16_t addr = KANTOKU_WIFI_CREDS_ADDR; addr < KANTOKU_CREDS_BLOCK_NULL_B; addr++) {
        byte = readByte(addr);
        if (byte == 0x00)
            break;
        else if (byte == ';')
            current = &pswd;
        else
            *current += byte;
    }

    // extract leased ip
    std::string ip4str;
    ip4str += std::to_string(readByte(KANTOKU_IP_ADDR));
    ip4str += '.';
    ip4str += std::to_string(readByte(KANTOKU_IP_ADDR + 1));
    ip4str += '.';
    ip4str += std::to_string(readByte(KANTOKU_IP_ADDR + 2));
    ip4str += '.';
    ip4str += std::to_string(readByte(KANTOKU_IP_ADDR + 3));

    // print info
    std::cout << "Header Bytes       : " << byteHex(readByte(0x0000)) << ' ' << byteHex(readByte(0x0001)) << '\n';
    std::cout << "Network Information:\n";
    std::cout << "  SSID: " << ssid << '\n';
    std::cout << "  PSWD: " << pswd << '\n';
    std::cout << "  IPv4: " << ip4str << '\n';
    std::cout << "Sensor Type        : " << byteHex(readByte(KANTOKU_SENSTYPE_ADDR)) << '\n';
    std::cout << "Paired UID         : " << (int)readByte(KANTOKU_PAIRED_UID_ADDR) << '\n';
}

void At28hc64::clean() {
    std::cout << "Cleaning EEPROM..." << std::endl;
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
    
    for (uint16_t addr = 0x0000; addr <= MAX_ADDR; addr++) {
        //std::cout << "\r   0x" << std::hex << std::setw(4) << std::setfill('0') << addr;
        writeByte(0x00, addr);
    }

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
    std::cout << " DONE!" << std::endl;
}

