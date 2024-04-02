class IPv4Addr:
    # create ipv4 addr from raw byte data
    def __init__(self, ipaddr: int):
        # split ip string into int of octets and save to addr
        self.addr = ipaddr
    
    # create ipv4 addr from string representation of ipv4
    @classmethod
    def fromString(cls, ipstr: str):
        return cls(IPv4Addr._createByteAddr([int(octet_str) for octet_str in ipstr.split('.')]))
    
    # stores octet values into member's address var
    def _createByteAddr(octets: list[int]) -> int:
        addr = 0x00000000
        addr |= octets[3]
        addr |= (octets[2] << 8)
        addr |= (octets[1] << 16)
        addr |= (octets[0] << 24)
        return addr
    
    @property
    def octetA(self) -> int:
        return (self.addr & 0xFF000000) >> 24
    @property
    def octetB(self) -> int:
        return (self.addr & 0x00FF0000) >> 16
    @property
    def octetC(self) -> int:
        return (self.addr & 0x0000FF00) >> 8
    @property
    def octetD(self) -> int:
        return self.addr & 0x000000FF

    # create string representation of ipv4 addr
    def __str__(self):
        return f'{self.octetA}.{self.octetB}.{self.octetC}.{self.octetD}'
    def __or__(self, other):
        return IPv4Addr(self.addr | other.addr)
    def __ror__(self, other):
        return self.__or__(other)

    def __and__(self, other):
        return IPv4Addr(self.addr & other.addr)
    def __eq__(self, other):
        if type(other) is IPv4Addr:
            return self.addr == other.addr
        else:
            return False
    def __add__(self, other):
        return IPv4Addr(self.addr + other)

class MacAddr:
    # create mac addr from string representation
    def __init__(self, addr: int):
        self.addr = addr
    
    @classmethod
    def fromString(cls, addr: str):
        return cls(cls._createByteAddr([int(octet_str, 16) for octet_str in addr.split(':')]))

    # store octets into class
    def _createByteAddr(octets: list[int]) -> int:
        addr = 0x00_00_00_00_00_00
        addr |= octets[5] << 40
        addr |= octets[4] << 32
        addr |= octets[3] << 24
        addr |= octets[2] << 16
        addr |= octets[1] << 8
        addr |= octets[0]
        return addr
    
    @property
    def octetA(self) -> int:
        return (self.addr & 0xff0000000000) >> 40
    @property
    def octetB(self) -> int:
        return (self.addr & 0x00ff00000000) >> 32
    @property
    def octetC(self) -> int:
        return (self.addr & 0x0000ff000000) >> 24
    @property
    def octetD(self) -> int:
        return (self.addr & 0x000000ff0000) >> 16
    @property
    def octetE(self) -> int:
        return (self.addr & 0x00000000ff00) >> 8
    @property
    def octetF(self) -> int:
        return self.addr & 0x0000000000ff

    # create string representation of mac address
    def __str__(self):
        return f'{self.octetA:02x}:{self.octetB:02x}:{self.octetC:02x}:{self.octetD:02x}:{self.octetE:02x}:{self.octetF:02x}'
