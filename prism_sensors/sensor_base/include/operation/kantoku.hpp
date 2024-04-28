#pragma once

#include "hardware/at28hc64.hpp"
#include "operation/serial/session.hpp"
#include "operation/sensors/upa.hpp"

extern "C" {
    #include <lwip/tcp.h>
}

// Byte values
#define KANTOKU_EEPROM_FORMATTED    0x00
#define KANTOKU_CREDS_SAVED         0x01
#define KANTOKU_NETCON_ESTABLISHED  0x02
#define KANTOKU_PAIRED_SUCCESSFULLY 0x03

// Address locations
#define KANTOKU_PAIR_STATUS_ADDR    0x02
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
        SkipPair,
        NoAction
    };

    struct kantoku_netinfo {
        uint8_t mac[6] = {0x00, 0x00, 0x00, 0x00};
        ip4_addr_t ctrlip;
        ip4_addr_t ip;
    } netinfo;

// member vars
private:
    EEPROM prom;
    Action action;
    ModuleType moduleType;
    UPASensor* upa;

private:
    Action determineAction();
    void formatEEPROM();
    void serialSetup();
    void networkConn();
    void parseSettinsJson(std::string rawJson, std::vector<std::pair<std::string, std::string>>& settingsBuf);
    std::vector<std::string> splitString(std::string str, char delimiter);

public:
    Kantoku(ModuleType moduleType);
    bool attemptPair();
    void attachUPA(UPASensor& s);
    void updateSensorInfo();
};