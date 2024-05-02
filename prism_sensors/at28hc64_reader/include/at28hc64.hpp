#pragma once

#include <cstdint>

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


// EEPROM Configuration
#define MAX_ADDR        0x0fff
#define DATA_BUS_MASK   0x000000ffu     //gpio 0-7
#define ADDR_BUS_MASK   0x1fffff00u     //gpio 8-20
#define OE 21u
#define WE 22u
#define CE 26u

class At28hc64 {
private:
    uint8_t outputEnable;
    uint8_t writeEnable;
    uint8_t chipEnable;

    void sleep();
    void oeSetHi();
    void oeSetLo();
    void weSetHi();
    void weSetLo();
    void ceSetHi();
    void ceSetLo();
    void gpioSetup();

public:
    At28hc64(uint8_t oeGpio, uint8_t weGpio, uint8_t ceGpio);
    At28hc64();

    uint8_t readByte(uint16_t addr);
    void writeByte(uint8_t byte, uint16_t addr);
    void printByte(uint8_t byte);

    void hexdump();
    void clean();
};