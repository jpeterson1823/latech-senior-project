#include <operation/serial/packet.hpp>

SerialPacket::SerialPacket(PacketType ptype, uint8_t* data, uint8_t dataLen) {
    this->ptype = ptype;
    this->dataLen = dataLen > SERPAC_DBUF_SIZE ? SERPAC_DBUF_SIZE : dataLen;
    for (uint8_t i = 0; i < this->dataLen; i++)
        this->data[i] = data[i];
}

SerialPacket::SerialPacket(uint8_t* rawBytes) {
    this->ptype = PacketType(rawBytes[2]);
    this->dataLen = rawBytes[4] > SERPAC_DBUF_SIZE ? SERPAC_DBUF_SIZE : rawBytes[4];
    for (uint8_t i = 0; i < this->dataLen; i++)
        this->data[i] = rawBytes[i];
}

void SerialPacket::getRaw(uint8_t* buffer, uint8_t* bufsize) {
    buffer[0] = header[0];
    buffer[1] = header[1];
    buffer[2] = ptype;
    buffer[3] = dataLen;
    *bufsize = dataLen+3;
    for (uint16_t i = 4; i < *bufsize+3; i++)
        buffer[i] = data[i-4];
}

uint8_t* SerialPacket::getData() {
    return this->data;
}

uint8_t SerialPacket::getType() {
    return this->ptype;
}