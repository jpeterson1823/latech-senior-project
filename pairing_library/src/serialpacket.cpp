#include <serialpacket.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

const uint8_t SerialPacket::header[2] = {0x23, 0x12};

SerialPacket::SerialPacket() {
    this->ptype = PacketType::NULLPACKET;
}

SerialPacket::SerialPacket(const SerialPacket& rhs) {
    ptype = rhs.ptype;
    plsize = rhs.plsize;
    for (uint8_t i = 0; i < plsize; i++)
        payload[i] = rhs.payload[i];
}

SerialPacket::SerialPacket(PacketType ptype) {
    this->ptype = ptype;
    this->plsize = 0;
}

SerialPacket::SerialPacket(PacketType ptype, uint8_t* data, uint8_t dataLen) {
    this->ptype = ptype;
    plsize = dataLen > SERPAC_DBUF_SIZE ? SERPAC_DBUF_SIZE : dataLen;
    for (uint8_t i = 0; i < plsize; i++)
        payload[i] = data[i];
}

SerialPacket::SerialPacket(uint8_t* rawBytes) {
    this->fromRaw(rawBytes);
}

void SerialPacket::fromRaw(uint8_t* raw) {
    this->ptype = PacketType(raw[2]);
    this->plsize = raw[3] > SERPAC_DBUF_SIZE ? SERPAC_DBUF_SIZE : raw[3];
    for (uint16_t i = 0; i < plsize; i++) {
        this->payload[i] = raw[i+4];
    }
    //this->ptype = PacketType(raw[2]);
    //this->plsize = raw[3];
    //for (uint8_t i = 0; i < plsize; i++) {
    //    std::cout << std::hex << raw[i] << std::endl;
    //    this->payload[i] = raw[i];
    //}
}

void SerialPacket::getRaw(uint8_t* buffer, uint8_t* bufsize) {
    buffer[0] = header[0];
    buffer[1] = header[1];
    buffer[2] = ptype;
    buffer[3] = plsize;
    *bufsize = plsize+4;
    for (uint16_t i = 4; i < *bufsize; i++)
        buffer[i] = payload[i-4];
}

uint8_t* SerialPacket::getPayloadBytes() {
    return this->payload;
}

uint8_t SerialPacket::getPayloadSize() {
    return this->plsize;
}

void SerialPacket::setType(PacketType t) {
    this->ptype = t;
}

PacketType SerialPacket::getType() {
    return this->ptype;
}


/**
 * @brief Load bytes into payload at specified offset
 * 
 * @param data 
 * @param dlen 
 * @param startIndex 
 * @return True if bytes fit, false otherwise
 */
bool SerialPacket::loadIntoPayload(uint8_t* data, uint8_t dlen, uint8_t offset) {
    // if offset is outside of packet size, data wont fit and can't be stored.
    if (offset >= plsize)
        return false;

    // place data in payload, starting at offset
    uint8_t di, pi;
    for (di = 0, pi = di+offset; di < dlen && pi < plsize; di++)
        payload[pi] = data[di];

    // check if all of data fit within the payload
    return dlen <= plsize - offset;
}
bool SerialPacket::loadIntoPayload(uint8_t* data, uint8_t dlen) {
    return loadIntoPayload(data, dlen, 0);
}

bool SerialPacket::loadFromPayload(uint8_t* buf, uint8_t offset, uint8_t nbytes) {
    // byte address is outside of payload length, not possible
    if (offset >= plsize)
        return false;
    
    uint8_t i, j;
    for (i = 0, j = i+offset; i < nbytes && j < plsize; i++) 
        buf[i] = payload[j];
    
    // check if all of data fit within the payload
    return nbytes <= plsize - offset;
}

bool SerialPacket::setPayloadByte(uint8_t addr, uint8_t byte) {
    // byte address is outside of payload length, not possible
    if (addr >= plsize)
        return false;
    
    payload[addr] = byte;
    return true;
}

uint8_t SerialPacket::getPayloadByte(uint8_t addr) {
    if (addr < plsize)
        return payload[addr];
    return 0x00;
}

std::string SerialPacket::toString() {
    std::stringstream s;

    s << "SerialPacket{header=[";
    s << std::hex << std::setfill('0') << std::setw(2) << (int)this->header[0];
    s << ",";
    s << std::hex << std::setfill('0') << std::setw(2) << (int)this->header[1];
    s << "],ptype=";
    s << std::hex << std::setfill('0') << std::setw(2) << (int)ptype;
    s << ",plsize=" << std::to_string(this->plsize);

    s << ",payload=[";
    for (uint8_t i = 0; i < plsize; i++)
        s << std::setw(2) << std::setfill('0') << std::hex << (int)payload[i] << ' ';
    s << "]}";

    return s.str();
}