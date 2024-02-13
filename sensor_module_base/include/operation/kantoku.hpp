#pragma once

#include "hardware/serial_interface.hpp"
#include "hardware/parallel_eeprom.hpp"

#define NOBLE6 2312
#define KANTOKU_ROM_START           0x03
#define KANTOKU_EEPROM_FORMATTED    0x00
#define KANTOKU_CREDS_SAVED         0x01

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
        NetworkPair,
        NoAction
    };

// member vars
private:
    EEPROM prom;
    Action action;

// member fn
private:
    Action determineAction();
    void formatEEPROM();
    void serialSetup();
    void networkPair();

public:
    Kantoku();
};