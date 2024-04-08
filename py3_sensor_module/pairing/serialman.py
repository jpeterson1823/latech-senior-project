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
            self._data = None
        else:
            self._data  = self._header
            self._data += bytes(self.ptype.value) 
            self._data += len(data).to_bytes(1, byteorder='big')
            self._data += data
    
    @classmethod
    def fromBytes(cls, data: bytes):
        if not data or data[0:2] != cls._header:
            return None
        
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
        if self._session:
            return not self._session.write(packet.raw(), timeout=self._timeout)
        print("[WARN] Cannot write data to closed serial session!")
        return False
    
    def recv(self) -> SerialPacket:
        if self._session:
            return SerialPacket.fromBytes(self._session.read_until(None, size=0xFF, timeout=self._timeout))