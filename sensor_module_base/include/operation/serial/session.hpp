#pragma once
#include <string>
#include <operation/serial/packet.hpp>

class SerialSession {
private:
    float timeout;
    bool sessionActive;
    bool packetReady;
    uint8_t* pbuf;

public:
    SerialSession();

    void setTimeout(float timeout);
    bool open();
    bool close();
    bool send(SerialPacket& packet);
    bool packetAvailable();
    bool isActive();
    void recv(SerialPacket& packet);

    static void __SerialSessionCore1Entry();
};