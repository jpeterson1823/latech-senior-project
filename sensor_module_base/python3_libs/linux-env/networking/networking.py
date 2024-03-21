class IPv4Addr:
    def __init__(self, a: int, b: int, c: int, d: int):
        self.a = a
        self.b = b
        self.c = c
        self.d = d
    
    def __str__(self):
        return f'{self.a}.{self.b}.{self.c}.{self.d}'

class MacAddr:
    def __init__(self, a: int, b: int, c: int, d: int, e: int, f: int):
        self.a = a
        self.b = b
        self.c = c
        self.d = d
        self.e = e
        self.f = f
    
    def __init__(self, octets: list):
        self.a = octets[0]
        self.b = octets[1]
        self.c = octets[2]
        self.d = octets[3]
        self.e = octets[4]
        self.f = octets[5]

    def __str__(self):
        return f'{self.a:02x}:{self.b:02x}:{self.c:02x}:{self.d:02x}:{self.e:02x}:{self.f:02x}'


class Module:
    def __init__(self, ipv4: IPv4Addr, mac: MacAddr):
        self.ip = ipv4
        self.mac = mac