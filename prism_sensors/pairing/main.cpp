// output formatting
#include <iostream>
#include <iomanip>

// for sleep
#include <chrono>
#include <thread>

// custom libs
#include "networking/addressing.hpp"
#include "networking/dhcpman.hpp"

// include pairing functions
#include "pairing.hpp"


int main() {
    // open serial session
    serial::openSession("/dev/ttyACM0", 115200);
    serial::session.setDTR();

    SerialPacket pbuf;
    pairing::pbuf = &pbuf;

    // do ident process
    std::cout << "doIdent()... ";
    if (!pairing::doIdent()) {
        std::cout << "FAIL! Device's response is below:\n";
        std::cout << pairing::pbuf->toString() << std::endl;
        return 1;
    }
    std::cout << "SUCCESS!" << std::endl;


    // do intent query process
    std::cout << "doIdentQ()... ";
    if (!pairing::doIntentQ()) {
        std::cout << "FAIL! Device's response is below:\n";
        std::cout << pairing::pbuf->toString() << std::endl;
        return 1;
    }
    std::cout << "SUCCESS!" << std::endl;

    // parse payload into Mac object
    Mac* mac = new Mac(
        pairing::pbuf->getPayloadByte(0x00),
        pairing::pbuf->getPayloadByte(0x01),
        pairing::pbuf->getPayloadByte(0x02),
        pairing::pbuf->getPayloadByte(0x03),
        pairing::pbuf->getPayloadByte(0x04),
        pairing::pbuf->getPayloadByte(0x05)
    );

    // create DHCP manager and create/retrieve module's lease
    std::cout << "leasing" << std::endl;
    DHCPMan dhcp;
    DHCPLease lease = dhcp.lease(mac);

    // send pairing response
    pairing::sendPairInfo(lease.getIP4());

    return 0;
}