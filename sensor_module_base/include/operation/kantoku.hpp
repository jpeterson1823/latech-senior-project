#pragma once

#include <hardware/parallel_eeprom.hpp>
#include <operation/serial/session.hpp>

extern "C" {
    #include <lwip/tcp.h>
}

#define NOBLE6 2312
#define KANTOKU_EEPROM_FORMATTED    0x00
#define KANTOKU_CREDS_SAVED         0x01
#define KANTOKU_PAIRED              0x02
#define KANTOKU_ROM_START           0x03

#define KANTOKU_WIFI_CREDS_ADDR     0x03
#define KANTOKU_WIFI_CREDS_BLOCKLEN 0x80    // 128 characters
#define KANTOKU_CREDS_BLOCK_NULL_B  0x83    // extra nullterm to make sure no overread
#define KANTOKU_IP_ADDR             0x84    // addr for leased ip
#define KANTOKU_SENSTYPE_ADDR       0x88

/*
Kantoku: Director of comms
Is responsible for controlling the flow of the program
*/
class Kantoku
{
private:
    // create Action enum to keep track of possible/current action
    enum Action {
        SerialSetup,
        NetworkConnect,
        NetworkPair,
        EstablishUplink,
        NoAction
    };

    struct kantoku_netinf {
        ip4_addr_t ctrlip;
        uint8_t mac[6];
        ip4_addr_t ip;
    } netinfo;

// member vars
private:
    Action action;
    ModuleType moduleType;

// member fn
private:
    Action determineAction();
    void formatEEPROM();
    void serialSetup();
    void networkConn();

public:
    EEPROM prom;
    Kantoku(ModuleType moduleType);
    bool attemptPair();
};