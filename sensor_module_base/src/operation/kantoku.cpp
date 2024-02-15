#include "operation/kantoku.hpp"
#include "hardware/serial_interface.hpp"
#include "networking/wifi.hpp"
#include "networking/sockets.hpp"

#include <string>
#include <iostream>

// sets up kantoku
Kantoku::Kantoku() {
    // controller ip set here. should always be the same
    //this->controllerAddrStr = "192.168.1.1";
    this->controllerAddrStr = "93.184.216.34";
    ip4addr_aton(controllerAddrStr.data(), &controllerAddr);

    //first, must determine what state kantoku should be in
    determineAction();

    if (this->action == Action::SerialSetup)
        serialSetup();
    if (this->action == Action::NetworkPair)
        networkPair();

};

// formats eeprom
void Kantoku::formatEEPROM() {
    // zero entire eeprom
    for (uint32_t i = 0; i <= 2048; i++)
        prom.writeByte(0x00, i);

    // first two bytes need to be noble-6 (HI:LO)
    prom.writeByte(0x09, 0x0000);
    prom.writeByte(0x08, 0x0001);
    // flag byte already at KANTOKU_EEPROM_FORMATTED, so no action needed there
}

// determines which action should be taken given current situation
Kantoku::Action Kantoku::determineAction() {
    // first things first, determine if PROM has been formatted via checking first & second byte
    uint16_t formatFlag = 0x0000;
    formatFlag |= (uint16_t)(prom.readByte(0x0000)) << 8;
    formatFlag |= (uint16_t)prom.readByte(0x0001);

    std::cout << "format flag: " << formatFlag << std::endl;

    // format eeprom if first two bytes not noble6
    if (formatFlag != NOBLE6) {
        std::cout << "Not Noble Enough. Cleaning EEPROM..." << std::endl;
        formatEEPROM();
        // set state to SerialSetup
        this->action = Action::SerialSetup;
        std::cout << "Cleaning finished. SerialSetup set as action." << std::endl;
        return this->action;
    }

    // if noble6, then check flag state and return appropriate action
    uint8_t flags = prom.readByte(0x0002);
    if (flags == KANTOKU_EEPROM_FORMATTED)
        this->action = Action::SerialSetup;
    else if (flags == KANTOKU_CREDS_SAVED)
        this->action = Action::NetworkPair;
    else if (flags == KANTOKU_PAIRED)
        this->action = Action::EstablishUplink;
    else
        this->action = Action::NoAction;
    
    // return current action
    return this->action;
}

// negotiates creds with controller
void Kantoku::serialSetup() {
    // begin connection by waiting for controller to send --dont care-- data
    std::string sbuf;
    serial_recv(sbuf);
    sbuf.clear();

    // send init req and start creds exchange
    serial_send("kenchiki.hajime");
    serial_recv(sbuf);

    // send ack to close connection
    serial_ack();

    sleep_ms(1000);

    // parse creds info
    uint16_t delim_index = sbuf.find(';');
    std::cout << "SBUF: '" << sbuf << '\'' << std::endl;
    std::string ssid = sbuf.substr(0, delim_index+1);
    std::cout << "SSID OBTAINED" << std::endl;
    std::string pswd = sbuf.substr(delim_index, sbuf.find('\n') - delim_index + 1);
    std::cout << "PSWD OBTAINED" << std::endl;

    // write creds to eeprom, making sure to write a \x00 at end of creds string
    std::string creds = ssid + ';' + pswd;
    prom.writeString(creds.c_str(), KANTOKU_ROM_START);
    prom.writeByte(0x00, KANTOKU_ROM_START + creds.length());

    // update flag on eeprom
    prom.writeByte(KANTOKU_CREDS_SAVED, 0x0002);

    // set action to network pair
    this->action = Action::NetworkPair;
}

// Connect to network using saved creds
void Kantoku::networkConn() {
    // retrieve creds from EEPROM
    char ssid[1024];
    char pswd[1024];
    uint16_t delimAddr = prom.readUntil(KANTOKU_ROM_START, ';', ssid, 1024);
    prom.readUntil(KANTOKU_ROM_START + delimAddr, '\0', pswd, 1024);
    sleep_ms(500);

    // setup and connect to wifi, 2 retry attempts
    int status = Wifi::Connect(ssid, pswd);
    for (uint8_t i = 0; i < 2; i++) {
        if (status != CYW43_LINK_UP) {
            std::cout << "Failed to connect to network with SSID: \"" << ssid << "\"\n";
            std::cout << "Retrying (attempt " << i + 1 << '/' << 2 << ')' << std::endl;
            status = Wifi::Connect(ssid, pswd);
        } else {
            std::cout << "Successfully connected to WiFi network." << std::endl;
            break;
        }
    }
    if (status != CYW43_LINK_UP) {
        std::cout << "Could not connect to WiFi network. Exiting..." << std::endl;
        this->action = Action::NoAction;
    } else {
        this->action = Action::NetworkPair;
    }
};

void Kantoku::networkPair() {
    // connect to network
    networkConn();
    sleep_ms(1000);

    // create http req string
    //std::string pairReq = "POST / HTTP/1.1\r\nHost: " + controllerAddrStr + "\r\n\r\n";
    std::string pairReq = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";

    // open socket w/controller and send req
    SocketTCP socket(&controllerAddr, 80);
    socket.sendStr(pairReq);

    while(true) { sleep_ms(1000); }
}
