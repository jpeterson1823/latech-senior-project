from serial import Serial
from enum import Enum

class ModuleType(Enum):
    NONE  = 0x00
    PHOTO = 0x01
    PIR   = 0x02
    UPA   = 0x03

class PacketType(Enum):
    ACK      = 0x00
    IDENT    = 0x01
    INTENT_Q = 0x02
    RESPONSE = 0x03
    PAIR_REQ = 0x04
    DATA_RAW = 0x05

class SerialPacket:
    def __init__(self, packetType: PacketType, data: bytes):
        self._header = b'\x23\x12'
        self._ptype = packetType

        if self._ptype == PacketType.IDENT:
            self._data = bytearray([])
        else:
            self._data  = bytearray(self._header)
            self._data += bytearray(self._ptype) 
            self._data += bytearray(len(data).to_bytes(1, byteorder='big'))
            self._data += bytearray(data)
    
    @classmethod
    def fromBytes(cls, data: bytes):
        packetType = data[2]
        datalen = data[3]
        
        return cls(packetType, data[4:])

    def raw(self) -> bytes:
        return self._data
    
    @property
    def data(self) -> bytes:
        return self._data[4:]
    
    @property
    def typeCode(self) -> int:
        return self._data[2]

    def __str__(self) -> str:
        return f"SerialPacket[header={self._data[0]:02x}{self._data[1]:02x}, pident={self._data[2]:02x}, len={self._data[3]}, data={str(self._data[4:])}]"




class SerialSession:
    def __init__(self, port: str, baud: int):
        self._port = port
        self._baud = baud
        self._session = None
        self._timeout = 5.0 # timeout in seconds

        self._buf = bytearray([])

    def _resetPacketBuf(self):
        self._buf = bytearray([])
    
    def setTimeout(self, timeout: float):
        self._timeout = timeout
    
    def open(self):
        if not self._session:
            self._session = Serial(self._port, self._baud)
    
    def close(self):
        if self._session:
            self._session.close()
            self._session = None
    
    def send(self, packet: SerialPacket) -> bool:
        print("SENDING PACKET... ", end='')
        if self._session:
            v = self._session.write(packet.raw())
            print(f"DONE with v={v}")
            return v
        print("[WARN] Cannot write data to closed serial session!")
        return False
    
    def recv(self) -> SerialPacket:
        print("RECVIGN PACKET... ", end='')
        if not self._session:
            print("[WARN::SerialSession] recv() called on closed session!")
            return None
        
        # reset buf
        self._buf = []
        # receive first bit of packet until length is received
        for i in range(4):
            print(f"reading byte {i+1}")
            self._buf += self._session.read(1)

        # read remaining payload
        payload_len = int(self._buf[3])
        self._buf += self._session.read(payload_len)

        # return serial packet
        print("DONE")
        return SerialPacket.fromBytes(self._buf)
