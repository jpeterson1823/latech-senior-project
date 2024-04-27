from networking.addressing import MacAddr, IPv4Addr
from uuid import getnode as get_mac

class DHCPMan:
    # create DHCP manager and create an entry for host machine
    def __init__(self):
        # network information
        self._netmask    = IPv4Addr.fromString('255.255.255.0')
        self._network    = IPv4Addr.fromString('192.168.0.0')
        self._gateway    = (self._netmask & self._network) + 1
        self._broadcast  = (self._netmask & self._network) + 255

        # list of reserved addresses
        self._reservedAddrs = [
            self._network,
            self._gateway,
            self._broadcast
        ]

        # dictionary of leases
        self._leases = {MacAddr(get_mac()) : self._gateway}

    # print leases to console
    def _show_leases(self) -> None:
        for ip,mac in self.leases:
            print(f'{mac} {ip}')
    
    # generate new unused IPv4Addr
    def _genIPv4Addr(self) -> IPv4Addr:
        newAddr = self._netmask & self._gateway

        while newAddr in self._leases.values() or newAddr in self._reservedAddrs:
            newAddr.addr += 1
        return newAddr
    
    # generate new ip and add it to lease registry
    def genLease(self, macAddr: MacAddr) -> IPv4Addr:
        # if mac addr not in registry, create new lease
        if not self._leases.get(macAddr):
            newAddr = self._genIPv4Addr()
            self._leases[macAddr] = newAddr
            return newAddr
    
    @property
    def leases(self) -> list[tuple[MacAddr, IPv4Addr]]:
        return [(k,v) for k,v in self._leases.items()]
    
if __name__ == "__main__":
    print("Running DHCPMan test")
    dhcpman = DHCPMan()
    dhcpman.addNewEntry(MacAddr.fromString("43:43:43:43:43:43"))

    dhcpman._show_leases()
