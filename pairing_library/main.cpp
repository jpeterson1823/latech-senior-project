// output formatting
#include <iostream>
#include <iomanip>

// for sleep
#include <chrono>
#include <thread>

// custom libs
#include "lib/serialib.h"
#include "serialpacket.hpp"
#include "networking/addressing.hpp"

// create global serial variable
static serialib serial;

void sendPacket(SerialPacket p) {
    // set Request To Send
    serial.setRTS();
    // send packet
    uint8_t buf[256];
    uint8_t blen;
    p.getRaw(buf, &blen);
    for (int i = 0; i < blen; i++) {
        serial.writeChar((char)buf[i]);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // clear RTS and DTR
    serial.clearRTS();
};

SerialPacket readPacket() {
    // create packet buffer
    uint8_t pbuf[SERPAC_DBUF_SIZE];

    // read first 4 bytes to get payload data
    serial.readBytes(pbuf, 4);
    // read remaining payload into pbuf
    serial.readBytes((char*)(pbuf)+4, pbuf[3]);
    
    // create serial packet from raw data and return
    return SerialPacket(pbuf);
};

bool interrogateDevice(SerialPacket p, PacketType expectedType, SerialPacket* responseBuf = NULL) {
    // send packet and get response from device
    sendPacket(p);
    SerialPacket r = readPacket();
    *responseBuf = r;

    // determine if response is of the correct type
    return r.getType() == expectedType;
}




int main() {
    IP4 ip4(std::string("192.168.0.1"));
    std::cout << ip4.toString() << std::endl;

    Mac mac(std::string("aa:aa:aa:aa:aa:bb"));
    std::cout << mac.toString() << std::endl;

    return 0;
    // open serial
    char errorOpening = serial.openDevice("COM3", 115200);
    serial.setDTR();

    if (errorOpening!=1) {
        std::cout << "Failed to open serial. exit code = " << (int)errorOpening << std::endl;
        return errorOpening;
    }
    std::cout << "CONNECTED" << std::endl;

    // create packet buffer for invalid responses
    SerialPacket* pbuf;

    // do ident process
    std::cout << "IDENT... ";
    if (!interrogateDevice(SerialPacket(PacketType::IDENT), PacketType::IDENT, pbuf)) {
        std::cout << "FAIL! Device's response is below:\n";
        std::cout << pbuf->toString() << std::endl;
    }
    std::cout << "SUCCESS!" << std::endl;


    // do intent query process
    std::cout << "INTENT_Q... ";
    if (!interrogateDevice(SerialPacket(PacketType::INTENT_Q), PacketType::PAIR_REQ, pbuf)) {
        std::cout << "FAIL! Device's response is below:\n";
        std::cout << pbuf->toString() << std::endl;
    }
    std::cout << "SUCCESS!" << std::endl;

    // get pairing info

    return 0;
}