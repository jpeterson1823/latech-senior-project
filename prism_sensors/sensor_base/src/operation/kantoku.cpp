#include "operation/kantoku.hpp"
#include "operation/serial/packet.hpp"
#include "operation/serial/session.hpp"
#include "networking/wifi.hpp"
#include "networking/sockets.hpp"
#include "networking/httpreqs.hpp"
#include "networking/neosocket.hpp"

extern "C" {
    #include <pico/stdlib.h>
    #include <pico/cyw43_arch.h>
}

#include <string>
#include <iostream>
#include <iomanip>

// sets up kantoku
Kantoku::Kantoku(ModuleType moduleType) {
    // set module type
    this->moduleType = moduleType;
    // set controller IP
    //IP4_ADDR(&netinfo.ctrlip, 192, 168, 0, 1);
    IP4_ADDR(&netinfo.ctrlip, 192, 168, 9, 228);
    IP4_ADDR(&netinfo.ip, 192, 168, 9, 229);
    // get and store mac address
    cyw43_wifi_get_mac(&cyw43_state, CYW43_ITF_STA, netinfo.mac);

    //std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)netinfo.mac[0] << ":";
    //std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)netinfo.mac[1] << ":";
    //std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)netinfo.mac[2] << ":";
    //std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)netinfo.mac[3] << ":";
    //std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)netinfo.mac[4] << ":";
    //std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)netinfo.mac[5] << std::endl;

    /*for (uint16_t i = 0x0000; i < KANTOKU_SENSTYPE_ADDR; i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)prom.readByte(i);
        std::cout << ' ';
    }
    std::cout << std::endl;*/

    //first, must determine what state kantoku should be in
    determineAction();

    if (this->action == Action::NoAction)
        exit(1);

    // go through managerial actions
    if (this->action == Action::SerialSetup)
        serialSetup();

    /*sleep_ms(5000);
    for (uint16_t i = 0x0000; i < KANTOKU_SENSTYPE_ADDR; i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)prom.readByte(i);
        std::cout << ' ';
    }
    std::cout << std::endl;*/
    
    if (this->action == Action::NetworkConnect || this->action == Action::SkipPair)
        networkConn();
};

// formats eeprom
void Kantoku::formatEEPROM() {
    // zero entire eeprom
    for (uint32_t i = 0; i <= 512; i++)
        prom.writeByte(0x00, i);

    // first two bytes need to be noble-6 (HI:LO)
    prom.writeByte(0x23, 0x0000);
    prom.writeByte(0x12, 0x0001);
    // flag byte already at KANTOKU_EEPROM_FORMATTED, so no action needed there
}

// determines which action should be taken given current situation
Kantoku::Action Kantoku::determineAction() {
    // first things first, determine if PROM has been formatted via checking first & second byte
    uint8_t formatFlag_h = prom.readByte(0x0000);
    uint8_t formatFlag_l = prom.readByte(0x0001);

    //std::cout << "0x0000: " << std::hex << (int)formatFlag_h << std::endl;
    //std::cout << "0x0001: " << std::hex << (int)formatFlag_l << std::endl;

    // format eeprom if first two bytes not noble6
    if (formatFlag_h != 0x23 || formatFlag_l != 0x12) {
        std::cout << "Not Noble Enough. Cleaning EEPROM..." << std::endl;
        formatEEPROM();
        // set state to SerialSetup
        this->action = Action::SerialSetup;
        std::cout << "Cleaning finished. SerialSetup set as action." << std::endl;
        return this->action;
    }

    // if noble, check pair status
    uint8_t pstat = prom.readByte(KANTOKU_PAIR_STATUS_ADDR);
    std::cout << "pstat = " << std::hex << std::setw(2) << std::setfill('0') << (int)pstat << std::endl;
    if (pstat == KANTOKU_EEPROM_FORMATTED)
        this->action = Action::SerialSetup;
    else if (pstat == KANTOKU_CREDS_SAVED)
        this->action = Action::NetworkConnect;
    else if (pstat == KANTOKU_PAIRED_SUCCESSFULLY)
        this->action = Action::SkipPair;
    else {
        this->action = Action::NoAction;
        prom.writeByte(KANTOKU_EEPROM_FORMATTED, KANTOKU_PAIR_STATUS_ADDR);
    }
    
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
        // receive packet once one is available
        SerialPacket packet;
        s.recv(packet);

        // handle packet
        if (packet.getType() == PacketType::IDENT) {
            SerialPacket ident(PacketType::IDENT, nullptr, 0);
            s.send(ident);
        }
        else if (packet.getType() == PacketType::INTENT_Q) {
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
            pr.setPayloadSize(6);
            pr.loadIntoPayload(netinfo.mac, 6);
            //std::cout << pr.toString() << std::endl;
            // send packet
            s.send(pr);
        }
        else if (packet.getType() == PacketType::RESPONSE) {
            // extract leased IP and store in netinfo
            IP_ADDR4(
                &netinfo.ip,
                packet.getPayloadByte(0),
                packet.getPayloadByte(1),
                packet.getPayloadByte(2),
                packet.getPayloadByte(3)
            );

            sleep_ms(2000);

            // remaining data is string in format "SSID;PSWD"
            std::string wifiInfo = "";
            for (uint8_t i = 0; i < packet.getPayloadSize()-4 && i < KANTOKU_WIFI_CREDS_BLOCKLEN; i++)
                wifiInfo += (char)packet.getPayloadByte(i+4);
            std::cout << "wifiInfo: " << wifiInfo << std::endl;
            

            for (char c : wifiInfo)
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)c << " ";
            std::cout << std::endl;

            // write this creds string to eeprom
            prom.writeString(wifiInfo.c_str(), KANTOKU_WIFI_CREDS_ADDR);
                // set nullchar at KANTOKU_CREDS_BLOCK_NULL_B
            prom.writeByte('\0', KANTOKU_CREDS_BLOCK_NULL_B);

            // write leased ip to eeprom
            prom.writeByte(ip4_addr_get_byte(&netinfo.ip, 0), KANTOKU_IP_ADDR);
            prom.writeByte(ip4_addr_get_byte(&netinfo.ip, 1), KANTOKU_IP_ADDR + 1);
            prom.writeByte(ip4_addr_get_byte(&netinfo.ip, 2), KANTOKU_IP_ADDR + 2);
            prom.writeByte(ip4_addr_get_byte(&netinfo.ip, 3), KANTOKU_IP_ADDR + 3);

            // write module type to eeprom
            prom.writeByte(moduleType, KANTOKU_SENSTYPE_ADDR);

            // set pair status byte to CREDS_SAVED
            prom.writeByte(KANTOKU_CREDS_SAVED, KANTOKU_PAIR_STATUS_ADDR);

            // halt loop as pairing is complete
            shouldLoop = false;
        }
        else {
            //std::cout << packet.getType() << " DEFAULT CASE KANTOKU" << std::endl;
            uint8_t buf[0xff];
            uint8_t bs;
            packet.getRaw(buf, &bs);
            for (int i = 0; i < bs; i++)
                std::cout << buf[i];
            std::cout << std::endl;
        }
    }
    s.close();

    this->action = Action::NetworkConnect;
}

// Connect to network using saved creds
void Kantoku::networkConn() {
    // retrieve creds from EEPROM
    char ssid[KANTOKU_WIFI_CREDS_BLOCKLEN];
    char pswd[KANTOKU_WIFI_CREDS_BLOCKLEN];
    uint16_t delimAddr = prom.readUntil(KANTOKU_WIFI_CREDS_ADDR, ';', ssid, KANTOKU_WIFI_CREDS_BLOCKLEN);
    prom.readUntil(delimAddr+1, '\0', pswd, KANTOKU_WIFI_CREDS_BLOCKLEN);
    sleep_ms(500);

    std::cout << "Attempting to connect to network with following creds:\n";
    std::cout << "  SSID: " << ssid << "\n  PSWD: " << pswd << std::endl;

    // setup and connect to wifi, 2 retry attempts
    int status = Wifi::Connect(ssid, pswd, &netinfo.ip);
    for (uint8_t i = 0; i < 2; i++) {
        if (status != CYW43_LINK_UP) {
            std::cout << "Failed to connect to network with SSID: \"" << ssid << "\"\n";
            std::cout << "Retrying (attempt " << i + 1 << '/' << 2 << ')' << std::endl;
            status = Wifi::Connect(ssid, pswd, &netinfo.ip);
        } else {
            std::cout << "Successfully connected to WiFi network." << std::endl;
            break;
        }
    }
    if (status != CYW43_LINK_UP) {
        std::cout << "Could not connect to WiFi network. Exiting..." << std::endl;
        this->action = Action::NoAction;
    } else {
        if (this->action != Action::SkipPair)
            this->action = Action::NetworkPair;
    }
};

bool Kantoku::attemptPair() {
    if (this->action == Action::SkipPair)
        return true;

    // get ip and mac addr
    std::string ipAddr(ip4addr_ntoa(&netinfo.ip));
    std::string macAddr;
    Wifi::GetMacString(macAddr);

    // get controller ip string
    std::string ctrlipstr(ip4addr_ntoa(&netinfo.ctrlip));

    // create uri and params string
    std::string uri = "/php/demo.php";
    std::string data = "ipaddr=" + ipAddr + "&macaddr=" + macAddr + "&sensor_name=upa1";

    // create pair HTTP request
    Http::PostReq req (
        ctrlipstr,
        uri,
        data
    );

    // open socket and send http req
    socket::initialize(&netinfo.ctrlip, 40553);
    socket::send(req);
    socket::wait();

    if (socket::dataAvailable()) {
        // pop response off of queue
        std::cout << socket::popRecvq() << std::endl;
        // set pair status byte to PAIRED_SUCCESSFULLY
        prom.writeByte(KANTOKU_PAIRED_SUCCESSFULLY, KANTOKU_PAIR_STATUS_ADDR);
        return true;
    }
    else return false;
}

void Kantoku::attachUPA(UPASensor& upa) {
    this->upa = &upa;
}


std::vector<std::string> Kantoku::splitString(std::string str, char delimiter) {
    size_t pos = 0;
    std::string token;
    std::vector<std::string> split;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
        split.push_back(token);
        str.erase(0, pos + 1);
    }
    return split;
}

void Kantoku::parseSettinsJson(std::string rawJson, std::vector<std::pair<std::string, std::string>>& settingsBuf) {
    // remove squiggle brackets from front and back
    rawJson.erase(0);
    rawJson.erase(rawJson.length()-1);

    // split json by commas
    for (std::string settingStr : splitString(rawJson, ',')) {
        std::vector<std::string> pair = splitString(settingStr, ':');
        // erase " from front and back
        pair[0].erase(0);
        pair[0].erase(pair[0].length()-1);
        pair[1].erase(0);
        pair[1].erase(pair[1].length()-1);
        // add pair to buffer
        settingsBuf.push_back(std::pair<std::string, std::string>(pair[0], pair[1]));
    }
}

void Kantoku::updateSensorInfo() {
    // make get request for current sensor's paired data with db
    std::string mac;
    Wifi::GetMacString(mac);
    Http::GetReq req (
        ip4addr_ntoa(&netinfo.ctrlip),
        "/php/demo.php",
        std::string("macaddr=") + mac
    );

    socket::initialize(&netinfo.ctrlip, 40553);
    socket::send(req);
    socket::wait();

    // pull json data out of GET response
    std::string httpRes = socket::popRecvq();
    std::string rawSensorSettings = httpRes.substr(httpRes.find("\r\n\r\n")+4, httpRes.length()-1);
    std::cout << rawSensorSettings << std::endl;

    // parse raw sensor settings into setting variables
    std::vector<std::pair<std::string, std::string>> settingsBuf;
    parseSettinsJson(rawSensorSettings, settingsBuf);
}
