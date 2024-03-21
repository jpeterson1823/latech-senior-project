from uuid import getnode as get_mac
from .networking import MacAddr, IPv4Addr

class DHCPMan:
    def __init__(self):
        self.leases = {'localhost' : str(self._create_mac_obj())}
    
    def _create_mac_obj(self) -> MacAddr:
        raw_mac = get_mac()
        octets = [
            (raw_mac & 0xFF_00_00_00_00_00) >> 40,
            (raw_mac & 0x00_FF_00_00_00_00) >> 32,
            (raw_mac & 0x00_00_FF_00_00_00) >> 24,
            (raw_mac & 0x00_00_00_FF_00_00) >> 16,
            (raw_mac & 0x00_00_00_00_FF_00) >> 8,
            (raw_mac & 0x00_00_00_00_00_FF)
        ]

        return MacAddr(octets)
    
    