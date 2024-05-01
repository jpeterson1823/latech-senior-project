#pragma once
#include <cstdint>
#include <string>

#define SERPAC_DBUF_SIZE 256

enum ModuleType : uint8_t {
    NONE  = 0x00,
    PHOTO = 0x01,
    PIR   = 0x02,
    UPA   = 0x03
};

enum PacketType : uint8_t {
    NULLPACKET     = 0xFF,
    ACK      = 0x00,
    IDENT    = 0x01,
    INTENT_Q = 0x02,
    RESPONSE = 0x03,
    PAIR_REQ = 0x04,
    DATA_RAW = 0x05
};

class SerialPacket {
private:
    static const uint8_t header[2];
    PacketType ptype;
    uint8_t payload[SERPAC_DBUF_SIZE];
    uint8_t plsize;

public:
    SerialPacket();
    SerialPacket(const SerialPacket& rhs);
    SerialPacket(uint8_t* rawBytes);
    SerialPacket(PacketType ptype, uint8_t* data, uint8_t dataLen);
    SerialPacket(PacketType ptype);

    void fromRaw(uint8_t* raw);
    void getRaw(uint8_t* buffer, uint8_t* bufsize);
    uint8_t* getPayloadBytes();
    uint8_t  getPayloadSize();
    void     setPayloadSize(uint8_t size);

    void     setType(PacketType t);
    PacketType  getType();

    bool loadFromPayload(uint8_t* buf, uint8_t offset, uint8_t nbytes);
    bool loadIntoPayload(uint8_t* data, uint8_t dlen, uint8_t offset);
    bool loadIntoPayload(uint8_t* data, uint8_t dlen);
    bool setPayloadByte(uint8_t addr, uint8_t byte);
    uint8_t getPayloadByte(uint8_t addr);


    std::string toString();
};