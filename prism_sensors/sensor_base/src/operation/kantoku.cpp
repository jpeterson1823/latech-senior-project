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
    this->uid = 1;
    // set controller IP
    //IP4_ADDR(&netinfo.ctrlip, 192, 168, 0, 1);
    //IP4_ADDR(&netinfo.ctrlip, 192, 168, 9, 228);
    IP4_ADDR(&netinfo.ctrlip, 192, 168, 9, 49);
    IP4_ADDR(&netinfo.gw, 192, 168, 9, 0);
    IP4_ADDR(&netinfo.mask, 255, 255, 255, 0);
    // get and store mac address
    cyw43_wifi_get_mac(&cyw43_state, CYW43_ITF_STA, netinfo.mac);

    //first, must determine what state kantoku should be in
    determineAction();

    // if action is set to NoAction
    if (this->action == Action::NoAction)
        exit(1);

    // go through managerial actions
    if (this->action == Action::SerialSetup)
        serialSetup();
    
    // start network connection
    if (this->action == Action::NetworkConnect || this->action == Action::CompleteStartup)
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

uint8_t Kantoku::getPairStatus() {
    return prom.readByte(KANTOKU_PAIR_STATUS_ADDR);
}

// Determines inital action for module base to start with
Kantoku::Action Kantoku::determineAction() {
    // format EEPROM if not noble
    if (!isNoble()) {
        formatEEPROM();
        // set next action to Serial Setup
        this->action = Action::SerialSetup;
        return this->action;
    }

    // check pair status
    uint8_t pstat = prom.readByte(KANTOKU_PAIR_STATUS_ADDR);

    // if EEPROM is formatted, action is serial setup
    if (pstat == KANTOKU_EEPROM_FORMATTED)
        this->action = Action::SerialSetup;
    
    // if network credits have been saved, start network connection
    else if (pstat == KANTOKU_CREDS_SAVED || pstat == KANTOKU_PAIRED_SUCCESSFULLY)
        this->action = Action::NetworkConnect;

    // Otherwise, an issue has occurred.
    else {
        this->action = Action::NoAction;
        prom.writeByte(KANTOKU_FORMAT_ERROR, KANTOKU_PAIR_STATUS_ADDR);
    }
    
    // return current action
    return this->action;
}

// Determines if EEPROM is Noble
bool Kantoku::isNoble() {
    // read first two bytes of EEPROM
    uint8_t noble[2];
    prom.readBytes(0x0000, noble, 2);

    // read pair status byte
    uint8_t pstat = prom.readByte(KANTOKU_PAIR_STATUS_ADDR);

    // determine nobility
    return (noble[0] == 0x23 && noble[1] == 0x12) && pstat != 0xFF;
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
            // extract wifi info from response packet
            std::string wifiInfo = "";
            for (uint8_t i = 0; i < packet.getPayloadSize()-5 && i < KANTOKU_WIFI_CREDS_BLOCKLEN; i++)
                wifiInfo += (char)packet.getPayloadByte(i+5);
            std::cout << "wifiInfo: " << wifiInfo << std::endl;
            
            // write this creds string to eeprom
            prom.writeString(wifiInfo.c_str(), KANTOKU_WIFI_CREDS_ADDR);
                // set nullchar at KANTOKU_CREDS_BLOCK_NULL_B
            prom.writeByte('\0', KANTOKU_CREDS_BLOCK_NULL_B);

            // save user id
            prom.writeByte(packet.getPayloadByte(4), KANTOKU_PAIRED_UID_ADDR);

            // write leased ip to eeprom
            prom.writeByte(packet.getPayloadByte(0), KANTOKU_IP_ADDR);
            prom.writeByte(packet.getPayloadByte(1), KANTOKU_IP_ADDR + 1);
            prom.writeByte(packet.getPayloadByte(2), KANTOKU_IP_ADDR + 2);
            prom.writeByte(packet.getPayloadByte(3), KANTOKU_IP_ADDR + 3);

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

    // load IP address into netinfo
    IP4_ADDR(
        &netinfo.ip,
        prom.readByte(KANTOKU_IP_ADDR),
        prom.readByte(KANTOKU_IP_ADDR + 1),
        prom.readByte(KANTOKU_IP_ADDR + 2),
        prom.readByte(KANTOKU_IP_ADDR + 3)
    );

    std::cout << "Attempting to connect to network with following creds:\n";
    std::cout << "  SSID: " << ssid << "\n  PSWD: " << pswd << std::endl;
    std::cout << "  IPv4: " << (int)ip4_addr_get_byte(&netinfo.ip, 0) << '.'
                            << (int)ip4_addr_get_byte(&netinfo.ip, 1) << '.'
                            << (int)ip4_addr_get_byte(&netinfo.ip, 2) << '.'
                            << (int)ip4_addr_get_byte(&netinfo.ip, 3) << std::endl;

    // setup and connect to wifi, 2 retry attempts
    int status = Wifi::Connect(ssid, pswd, &netinfo.ip, &netinfo.mask, &netinfo.gw);
    for (uint8_t i = 0; i < 2; i++) {
        if (status != CYW43_LINK_UP) {
            std::cout << "Failed to connect to network with SSID: \"" << ssid << "\"\n";
            std::cout << "Retrying (attempt " << i + 1 << '/' << 2 << ')' << std::endl;
            status = Wifi::Connect(ssid, pswd, &netinfo.ip, &netinfo.mask, &netinfo.gw);
        } else {
            std::cout << "Successfully connected to WiFi network." << std::endl;
            break;
        }
    }
    if (status != CYW43_LINK_UP) {
        std::cout << "Could not connect to WiFi network. Exiting..." << std::endl;
        this->action = Action::NoAction;
    }
    else this->action = Action::CompleteStartup;
};

bool Kantoku::attemptPair() {
    // get ip and mac addr
    std::string ipAddr(ip4addr_ntoa(&netinfo.ip));
    std::string macAddr;
    Wifi::GetMacString(macAddr);

    // get controller ip string
    std::string ctrlipstr(ip4addr_ntoa(&netinfo.ctrlip));

    // create uri and params string
    std::string uri = "/php/demo.php";
    std::string data = "ipaddr=" + ipAddr + "&macaddr=" + macAddr + "&uid=" + std::to_string((int)uid);

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

void Kantoku::parseSettinsJson(std::string rawJson, std::map<std::string, std::string>& settingsBuf) {
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
        settingsBuf.insert({pair[0], pair[1]});
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
    if (httpRes.length() == 0)
        return;
    std::string rawSensorSettings = httpRes.substr(httpRes.find("\r\n\r\n")+4, httpRes.length()-1);

    // if http data is empty, no update
    if (rawSensorSettings.length() == 0)
        return;

    // parse raw sensor settings into setting variables
    std::map<std::string, std::string> smap;
    parseSettinsJson(rawSensorSettings, smap);
    
    // update sensor settings
    upa::config cfg  = {
        std::stof(smap["lFovLimit"]),
        std::stof(smap["rFovLimit"]),
        std::stoul(smap["adcGate"])
    };
    upa->configure(cfg);

    // write configuration to EEPROM
    prom.writeFloat(cfg.lFovLimit,              KANTOKU_UPA_LFOV_LIMIT_ADDR);
    prom.writeFloat(cfg.rFovLimit,              KANTOKU_UPA_RFOV_LIMIT_ADDR);
    prom.writeByte((cfg.adcGate & 0xFF00) >> 8, KANTOKU_UPA_ADC_GATE_ADDR);
    prom.writeByte( cfg.adcGate & 0x00FF,       KANTOKU_UPA_ADC_GATE_ADDR);
}

void Kantoku::alertController() {
    // generate HTTP Post
    Http::PostReq req = {
        std::string(ip4addr_ntoa(&netinfo.ctrlip)),
        "/php/demo.php",
        "UPA ALERT"
    };
    // send alert
    socket::send(req);
}

void Kantoku::mainLoop() {
    std::vector<upa::result> scan;

    for (;;) {
        // scan fov
        scan = upa->sweepScan();

        // get closest ping
        upa::result closest = scan[0];
        for (upa::result r : scan) {
            if (closest.distance < r.distance)
                closest = r;
        }
        
        // print closest angle
        std::cout << "Closest Echo: " << closest.distance << "mm @ ";
        std::cout << closest.angle << "deg" << std::endl;

        // generate and send http post for data
        std::string pingData = std::to_string(closest.distance) + '&' + std::to_string(closest.angle);
        Http::PostReq post(
            std::string(ip4addr_ntoa(&netinfo.ctrlip)),
            "/php/proxy.php",
            pingData
        );
        socket::send(post);
        socket::wait();

        if (socket::dataAvailable())
            socket::popRecvq();
    }
}
