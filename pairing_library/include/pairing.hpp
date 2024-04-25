#pragma once
#include <cstdint>
#include <thread>
#include <chrono>
#include "lib/serialib.h"
#include "serialpacket.hpp"

namespace serial {
    inline static serialib session;

    void openSession(const char* port, const uint baud) {
        // open serial
        char errorOpening = session.openDevice(port, baud);
        if (errorOpening!=1) {
            std::cout << "Failed to open serial. exit code = " << (int)errorOpening << std::endl;
            exit(errorOpening);
        }
    }

    void sendPacket(SerialPacket p) {
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
        session.readBytes((char*)(pbuf)+4, pbuf[3]);
        
        // create serial packet from raw data and return
        return SerialPacket(pbuf);
    };

    bool interrogate(SerialPacket p, PacketType expectedType, SerialPacket* responseBuf = NULL) {
        // send packet and get response from device
        sendPacket(p);
        SerialPacket r = readPacket();
        *responseBuf = r;

        // determine if response is of the correct type
        return r.getType() == expectedType;
    }
}

namespace pairing {
    SerialPacket* pbuf;
    bool doIdent() {
        return serial::interrogate(SerialPacket(PacketType::IDENT), PacketType::IDENT, pbuf);
    }

    bool doIntentQ() {
        return serial::interrogate(SerialPacket(PacketType::INTENT_Q), PacketType::PAIR_REQ, pbuf);
    }
}