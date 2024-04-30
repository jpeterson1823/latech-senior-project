#pragma once
#include <cstdint>
#include <thread>
#include <chrono>
#include "lib/serialib.h"
#include "serialpacket.hpp"
#include "networking/dhcpman.hpp"
#include "networking/addressing.hpp"

namespace serial {
    inline static serialib session;

    void openSession(const char* port, const unsigned int baud) {
        // open serial
        char errorOpening = session.openDevice(port, baud);
        if (errorOpening!=1) {
            std::cout << "Failed to open serial. exit code = " << (int)errorOpening << std::endl;
            exit(errorOpening);
        }
        // set DTR
        session.setDTR();
    }

    void sendPacket(SerialPacket& p) {
        // set Request To Send
        session.setRTS();
        // send packet
        uint8_t buf[256];
        uint8_t blen;
        p.getRaw(buf, &blen);
        for (int i = 0; i < blen; i++) {
            session.writeChar((char)buf[i]);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        // clear RTS and DTR
        session.clearRTS();
    };

    SerialPacket readPacket() {
        // create packet buffer
        uint8_t pbuf[SERPAC_DBUF_SIZE];

        // read first 4 bytes to get payload data
        session.readBytes(pbuf, 4);

        // read remaining payload into pbuf
        if (pbuf[3] != 0)
            session.readBytes((char*)(pbuf+4), pbuf[3], 1000);
        
        // create serial packet from raw data and return
        return SerialPacket(pbuf);
    };

    bool interrogate(SerialPacket p, PacketType expectedType, SerialPacket* responseBuf = NULL) {
        // send packet and get response from device
        sendPacket(p);
        SerialPacket r = readPacket();
        *responseBuf = r;

        // determine if response is of the correct type
        return (int)r.getType() == (int)expectedType;
    }
}

namespace pairing {
    static SerialPacket* pbuf;

    bool doIdent() {
        return serial::interrogate(SerialPacket(PacketType::IDENT), PacketType::IDENT, pbuf);
    }

    bool doIntentQ() {
        return serial::interrogate(SerialPacket(PacketType::INTENT_Q), PacketType::PAIR_REQ, pbuf);
    }

    void sendPairInfo(IP4* ip4, uint32_t uid) {
        // extract IP address from lease object
        uint8_t ipOctets[4];
        ip4->getOctets(ipOctets);
        
        // create response packet
        std::string netCreds = "bingus;FizzBuzz23!";
        SerialPacket p(PacketType::RESPONSE);
        p.setPayloadSize(5+netCreds.size());
        p.loadIntoPayload(ipOctets, 4);
        p.setPayloadByte(0x04, (uint8_t)uid);
        p.loadIntoPayload((uint8_t*)netCreds.c_str(), netCreds.size(), 0x05);

        std::cout << "Packet to send: " << p.toString() << std::endl;

        // send response packet
        serial::sendPacket(p);
    }
}