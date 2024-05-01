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


int main(int argc, char* argv[]) {
    // get username from args; set default_user if none provided.
    uint32_t uid;
    if (argc > 1)
        uid = std::stoul(argv[1]);
    else
        uid = 1;

    // open serial session
    serial::openSession("/dev/ttyACM0", 115200);
    serial::session.setDTR();

    // create pairing buffer
    pairing::pbuf = (SerialPacket*)malloc(sizeof(SerialPacket));

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

    std::cout << pairing::pbuf->toString() << std::endl;

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
    std::cout << "Generating new lease for " << mac->toString() << "... " << std::endl;
    DHCPMan dhcp;
    DHCPLease* lease = dhcp.lease(mac);
    std::cout << "Lease generated. Assigned IPv4: " << lease->getIP4()->toString() << std::endl;

    // send pairing response
    std::cout << "Sending pairing info... " << std::endl;
    pairing::sendPairInfo(lease->getIP4(), uid);
    std::cout << "Pairing info sent. USB pairing completed." << std::endl;

    delete lease;
    free(pairing::pbuf);

    return 0;
}