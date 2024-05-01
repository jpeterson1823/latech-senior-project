from serial import Serial
from enum import Enum
from time import sleep

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
            self._data += bytearray(self._ptype.value) 
            self._data += bytearray(len(data).to_bytes(1, byteorder='big'))
            self._data += bytearray(data)
    
    def raw(self) -> bytes:
        return (self._header + bytes(self._ptype.value) + self._data)
    
    @property
    def data(self) -> bytes:
        return self._data[4:]
    
    @property
    def typeCode(self) -> int:
        return self._data[2]

    def __str__(self) -> str:
        s = f"SerialPacket[{self._data.hex()}]"
        return s




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
            self._session = Serial(
                port=self._port,
                baudrate=self._baud,
                parity='N',
                stopbits=1,
                bytesize=8
            )
    
    def close(self):
        if self._session:
            self._session.close()
            self._session = None
    
    def send(self, packet: SerialPacket) -> bool:
        if self._session:
            for b in packet.raw():
                self._session.write(b)
                sleep(0.01)
            return True
        print("[WARN] Cannot write data to closed serial session!")
        return False
    
    def recv(self) -> bytearray:
        if not self._session:
            print("[WARN::SerialSession] recv() called on closed session!")
            return None
        
        # reset buf
        self._buf = bytearray([])
        # receive first bit of packet until length is received
        for i in range(4):
            print("READING BYTE")
            self._buf += bytes(self._session.read())

        return self._buf
