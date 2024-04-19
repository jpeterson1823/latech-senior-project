#include <operation/kantoku.hpp>
#include <operation/serial/packet.hpp>
#include <operation/serial/session.hpp>
#include <networking/wifi.hpp>
#include <networking/sockets.hpp>
#include <networking/httpreqs.hpp>
#include <networking/neosocket.hpp>

extern "C" {
    #include <pico/stdlib.h>
    #include <pico/cyw43_arch.h>
}

#include <string>
#include <iostream>

// sets up kantoku
Kantoku::Kantoku(ModuleType moduleType) {
    // set module type
    this->moduleType = moduleType;
    // set controller IP
    IP4_ADDR(&netinfo.ctrlip, 192, 168, 0, 1);
    // get and store mac address
    cyw43_wifi_get_mac(&cyw43_state, CYW43_ITF_STA, netinfo.mac);

    //first, must determine what state kantoku should be in
    determineAction();

    // go through managerial actions
    if (this->action == Action::SerialSetup)
        serialSetup();
    if (this->action == Action::NetworkConnect)
        networkConn();

};

// formats eeprom
void Kantoku::formatEEPROM() {
    // zero entire eeprom
    for (uint32_t i = 0; i <= 512; i++)
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
        this->action = Action::NetworkConnect;
    else if (flags == KANTOKU_PAIRED)
        this->action = Action::EstablishUplink;
    else
        this->action = Action::NoAction;
    
    // return current action
    return this->action;
}

// negotiates creds with controller
void Kantoku::serialSetup() {
    // open serial session
    SerialSession s;
    s.open();

    // loop until successful pair
    bool shouldLoop = true;
    while (shouldLoop) {
        // wait for available packet
        while(!s.packetAvailable()) {
            //cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
            //sleep_ms(250);
            //cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
            //sleep_ms(250);
        }

        // receive packet once one is available
        SerialPacket packet;
        s.recv(packet);

        // handle packet
        switch(packet.getType()) {
            // IDENT if requested
            case PacketType::IDENT:
            {
                SerialPacket ident(PacketType::IDENT, nullptr, 0);
                s.send(ident);
                break;
            }

            // state intent if requested
            case PacketType::INTENT_Q:
            {
                /*
                Create pair request packet. Data Order:
                BYTE    |   VALUE
                0x00    |   0x23
                0x01    |   0x12
                0x02    |   PacketType::PAIR_REQ
                0x03    |   Payload Length
                0x04... |   Payload

                Payload Structure
                0x00-0x06 = Mac Addr
                     0x07 = SensorType
                */

                // create pair request packet
                SerialPacket pr(PacketType::PAIR_REQ);

                // load mac into payload
                pr.loadIntoPayload(netinfo.mac, 6);
                pr.setPayloadByte(0x07, ModuleType::NONE);
                // send packet
                s.send(pr);
                break;
            }
            
            // record response and pair info
            case PacketType::RESPONSE:
            {
                // extract leased IP and store in netinfo
                IP_ADDR4(
                    &netinfo.ip,
                    packet.getPayloadByte(0),
                    packet.getPayloadByte(1),
                    packet.getPayloadByte(2),
                    packet.getPayloadByte(3)
                );

                // remaining data is string in format "SSID;PSWD"
                std::string wifiInfo;
                for (uint8_t i = 4; i < packet.getPayloadSize() && i-4 < KANTOKU_WIFI_CREDS_BLOCKLEN; i++)
                    wifiInfo += char(packet.getPayloadByte(i));
               
                // write this creds string to eeprom
                prom.writeString(wifiInfo.c_str(), KANTOKU_WIFI_CREDS_ADDR);
                 // append nullchar to end of WiFi info and at KANTOKU_CREDS_BLOCK_NULL_B
                prom.writeByte(wifiInfo.size() + KANTOKU_WIFI_CREDS_ADDR, '\0');
                prom.writeByte(KANTOKU_CREDS_BLOCK_NULL_B, '\0');

                // write leased ip to eeprom
                prom.writeByte(ip4_addr_get_byte(&netinfo.ip, 0), KANTOKU_IP_ADDR);
                prom.writeByte(ip4_addr_get_byte(&netinfo.ip, 1), KANTOKU_IP_ADDR + 1);
                prom.writeByte(ip4_addr_get_byte(&netinfo.ip, 2), KANTOKU_IP_ADDR + 2);
                prom.writeByte(ip4_addr_get_byte(&netinfo.ip, 3), KANTOKU_IP_ADDR + 3);

                // write module type to eeprom
                prom.writeByte(moduleType, KANTOKU_SENSTYPE_ADDR);

                // halt loop as pairing is complete
                shouldLoop = false;
                break;
            }
        }
    }
    s.close();
}

// Connect to network using saved creds
void Kantoku::networkConn() {
    // retrieve creds from EEPROM
    char ssid[KANTOKU_WIFI_CREDS_BLOCKLEN];
    char pswd[KANTOKU_WIFI_CREDS_BLOCKLEN];
    uint16_t delimAddr = prom.readUntil(KANTOKU_WIFI_CREDS_ADDR, ';', ssid, KANTOKU_WIFI_CREDS_BLOCKLEN);
    prom.readUntil(delimAddr+1, '\0', pswd, 1024);
    sleep_ms(500);

    std::cout << "Attempting to connect to network with following creds:\n";
    std::cout << "  SSID: " << ssid << "\n  PSWD: " << pswd << std::endl;

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

bool Kantoku::attemptPair() {
    // get mac addr
    std::string macAddr;
    Wifi::GetMacString(macAddr);

    // get controller ip string
    std::string ctrlipstr(ip4addr_ntoa(&netinfo.ctrlip));

    // create uri and params string
    std::string uri = "/php/demo.php";
    std::string data = "REQ=PAIR&DATA=" + macAddr;

    // create pair HTTP request
    Http::PostReq req (
        ctrlipstr,
        uri,
        data
    );

    // open socket and send http req
    socket::initialize(&netinfo.ctrlip, 80);
    socket::send(req);
    socket::wait();

    return socket::dataAvailable();
}
