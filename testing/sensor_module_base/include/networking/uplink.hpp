#pragma once
#include "networking/sockets.hpp"

// (S-B312 N6) Remember Reach
#define UPLINK_INIT_PORT 2312
#define UPLINK_EOT "\xe2\x90\x84"

class Uplink {
private:
    uint8_t mac[6];
    SocketTCP link;

public:
    Uplink(ip4_addr_t* rhost);

    uint8_t pair();
    uint8_t status();
};