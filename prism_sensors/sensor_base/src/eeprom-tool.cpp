#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include "hardware/at28hc64.hpp"
extern "C" {
    #include <pico/cyw43_arch.h>
}

// Byte values
#define KANTOKU_EEPROM_FORMATTED    0x00
#define KANTOKU_CREDS_SAVED         0x01
#define KANTOKU_NETCON_ESTABLISHED  0x02
#define KANTOKU_PAIRED_SUCCESSFULLY 0x03
#define KANTOKU_FORMAT_ERROR        0xFF

// Address locations
#define KANTOKU_PAIR_STATUS_ADDR    0x0002
#define KANTOKU_WIFI_CREDS_ADDR     0x0003
#define KANTOKU_WIFI_CREDS_BLOCKLEN 0x0080  // 128 characters
#define KANTOKU_CREDS_BLOCK_NULL_B  0x0083  // extra nullterm to make sure no overread
#define KANTOKU_IP_ADDR             0x0084  //addr for leased ip
#define KANTOKU_SENSTYPE_ADDR       0x0088
#define KANTOKU_PAIRED_UID_ADDR     0x0089

// UPA Address locations
#define KANTOKU_UPA_BLOCK_START     0x0010  // start of UPA config block
#define KANTOKU_UPA_LFOV_LIMIT_ADDR 0x0010  // 4 bytes for a float
#define KANTOKU_UPA_RFOV_LIMIT_ADDR 0x0014  // 4 bytes for a float
#define KANTOKU_UPA_ADC_GATE_ADDR   0x0018  // 2 bytes for uint16_t
#define KANTOKU_UPA_BLOCK_SIZE      0x000B  // total size of upa config block (4+4+2=10 bytes)


std::string byteHex(uint8_t byte);
void printByte(uint8_t byte);
void hexdump(At28hc64b& prom, uint16_t startAddr, uint16_t endAddr);
void decypher(At28hc64b& prom);
void clean(At28hc64b& prom);

int main() {
    stdio_init_all();
    cyw43_arch_init();

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
    sleep_ms(2000);

    At28hc64b prom;
    hexdump(prom, 0x0000, 0x00FF);
    decypher(prom);

    cyw43_arch_deinit();
    return 0;
}



std::string byteHex(uint8_t byte) {
    std::stringstream s;
    s << std::hex << std::setw(2) << std::setfill('0') << (uint16_t)byte;
    return s.str();
}
void printByte(uint8_t byte) { std::cout << byteHex(byte); }
void hexdump(At28hc64b& prom, uint16_t startAddr, uint16_t endAddr) {
    std::cout.flush();
    std::cout << std::endl;
    std::cout << "B# 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "   ";
    for (uint16_t addr = startAddr; addr <= endAddr; addr++) {
        if (addr % 16 == 0 && addr != 0x0000) {
            std::cout << "\n   ";
            printByte(prom.readByte(addr));
            std::cout << ' ';
        }
        else {
            printByte(prom.readByte(addr));
            std::cout << ' ';
        }
    }
    std::cout << std::endl;
}
void decypher(At28hc64b& prom) {
    // extract ssid and pswd
    std::string ssid, pswd;
    std::string* current = &ssid;
    uint8_t byte;
    for (uint16_t addr = KANTOKU_WIFI_CREDS_ADDR; addr < KANTOKU_CREDS_BLOCK_NULL_B; addr++) {
        byte = prom.readByte(addr);
        if (byte == 0x00)
            break;
        else if (byte == ';')
            current = &pswd;
        else
            *current += byte;
    }

    // extract leased ip
    std::string ip4str;
    ip4str += std::to_string(prom.readByte(KANTOKU_IP_ADDR));
    ip4str += '.';
    ip4str += std::to_string(prom.readByte(KANTOKU_IP_ADDR + 1));
    ip4str += '.';
    ip4str += std::to_string(prom.readByte(KANTOKU_IP_ADDR + 2));
    ip4str += '.';
    ip4str += std::to_string(prom.readByte(KANTOKU_IP_ADDR + 3));

    // print info
    std::cout << "Header Bytes       : " << byteHex(prom.readByte(0x0000)) << ' ' << byteHex(prom.readByte(0x0001)) << '\n';
    std::cout << "Network Information:\n";
    std::cout << "  SSID: " << ssid << '\n';
    std::cout << "  PSWD: " << pswd << '\n';
    std::cout << "  IPv4: " << ip4str << '\n';
    std::cout << "Sensor Type        : " << byteHex(prom.readByte(KANTOKU_SENSTYPE_ADDR)) << '\n';
    std::cout << "Paired UID         : " << (int)prom.readByte(KANTOKU_PAIRED_UID_ADDR) << '\n';
}
void clean(At28hc64b& prom) {
    std::cout << "Cleaning EEPROM..." << std::endl;
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
    
    for (uint16_t addr = 0x0000; addr <= KANTOKU_PAIRED_UID_ADDR; addr++) {
        if (addr % 0x0008 == 0) {
            std::cout << "\r   0x" << std::hex << std::setw(4) << std::setfill('0') << addr;
            std::cout.flush();
        }
        prom.writeByte(0x00, addr);
    }

    for (uint16_t addr = KANTOKU_UPA_BLOCK_SIZE; addr <= KANTOKU_UPA_BLOCK_SIZE + KANTOKU_UPA_BLOCK_START; addr++) {
        if (addr % 0x0008 == 0) {
            std::cout << "\r   0x" << std::hex << std::setw(4) << std::setfill('0') << addr;
            std::cout.flush();
        }
        prom.writeByte(0x00, addr);
    }

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
    std::cout << " DONE!" << std::endl;
}