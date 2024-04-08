#pragma once
#include <cstdint>

#define SERPAC_DBUF_SIZE 256

enum ModuleType {
    NONE  = 0x00,
    PHOTO = 0x01,
    PIR   = 0x02,
    UPA   = 0x03
};

enum PacketType {
    ACK      = 0x00,
    IDENT    = 0x01,
    INTENT_Q = 0x02,
    RESPONSE = 0x03,
    PAIR_REQ = 0x04,
    DATA_RAW = 0x05
};

class SerialPacket {
private:
    PacketType ptype;
    uint8_t data[SERPAC_DBUF_SIZE];
    uint8_t dataLen;

public:
    const uint8_t header[2] = {0x23, 0x12};
    SerialPacket();
    SerialPacket(PacketType ptype, uint8_t* data, uint8_t dataLen);
    SerialPacket(uint8_t* rawBytes);

    void fromRaw(uint8_t* buffer);
    void getRaw(uint8_t* buffer, uint8_t* bufsize);
    uint8_t* getData();
    uint8_t  getDataLen();
    uint8_t  getType();
};