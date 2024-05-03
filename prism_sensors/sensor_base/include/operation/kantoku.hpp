#pragma once

#include "hardware/at28hc64.hpp"
#include "operation/serial/session.hpp"
#include "operation/sensors/upa.hpp"

extern "C" {
    #include <lwip/tcp.h>
}
#include <map>

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
        CompleteStartup,
        NoAction
    };

    struct kantoku_netinfo {
        uint8_t mac[6] = {0x00, 0x00, 0x00, 0x00};
        ip4_addr_t ctrlip;
        ip4_addr_t ip;
        ip4_addr_t mask;
        ip4_addr_t gw;
    } netinfo;

// member vars
private:
    At28hc64b prom;
    Action action;
    ModuleType moduleType;
    UPASensor* upa;
    uint8_t uid;

private:
    Action determineAction();
    bool isNoble();
    void formatEEPROM();
    void serialSetup();
    void networkConn();
    void parseSettinsJson(std::string rawJson, std::map<std::string, std::string>& settingsBuf);
    std::vector<std::string> splitString(std::string str, char delimiter);

public:
    Kantoku(ModuleType moduleType);
    uint8_t getPairStatus();
    bool attemptPair();
    void attachUPA(UPASensor& s);
    void updateSensorInfo();
    void alertController();

    void mainLoop();
};