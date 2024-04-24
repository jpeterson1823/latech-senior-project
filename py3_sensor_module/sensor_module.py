#!/bin/env/python3
import re, subprocess
from enum import Enum
from pairing.serialman import SerialSession, SerialPacket, PacketType, ModuleType
from pairing.database import Database, SensorEntry
from networking.dhcpman import DHCPMan
from networking.addressing import MacAddr, IPv4Addr


def pair(port: str, db: Database):
    # open serial session
    session = SerialSession(port, 115200)
    session.open()

    # send IDENT packet and confirm device is a module
    ident = SerialPacket(PacketType.IDENT, None)
    session.send(ident)

    packet = session.recv()
    if packet.typeCode != PacketType.IDENT.value:
        print(f"Device did not ident properly. Response Packet: {str(packet)}")
        print("Exiting...")
        exit()
    
    # device ident'd properly, ask for intention
    session.send(SerialPacket(PacketType.INTENT_Q, None))
    packet = session.recv()
    print(str(packet))

    # pair with module if requested
    if packet.typeCode == PacketType.PAIR_REQ:
        # start DHCP manager
        dhcpman = DHCPMan()

        # extract requisite data from packet and create dhcp lease
        macAddr = MacAddr(packet.data[0:6])
        sensorType = packet.data[7]
        leasedIP = dhcpman.genLease(macAddr)

        # create and insert db entry
        #entry = SensorEntry(sensorType, macAddr, leasedIP)
        #db.query(entry)

        # generate response packet; data is in following order: SSID;PASSWD;IPADDR
        data = leasedIP.rawBytes
        data += bytearray("bingus;FizzBuzz23!")
        leasePacket = SerialPacket(PacketType.RESPONSE, data)

        # send pairing packet to module and close connection
        session.send(leasePacket)
        session.close()
    else:
        print("Request type not yet implemented")

if __name__ == "__main__":
    #db = Database("localhost", "usr", "123")
    #pair("/dev/ttyACM0", None)
    #pair("com3", None)

    s = SerialSession("com3", 115200)
    s.open()

    ident = SerialPacket(PacketType.IDENT, [])
    print("IDENT: " + ident.raw().hex())

    print("SENDING PACKET....", end='')
    s.send(ident)
    print("DONE")

    print("RECVING PACKET...", end='')
    recv = s.recv()
    print("DONE")

    print("RAW PACKET DATA:")
    print(recv.hex())

    s.close()
