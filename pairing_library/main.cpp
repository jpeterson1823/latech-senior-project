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
    serial::openSession("COM3", 115200);
    serial::session.setDTR();

    // do ident process
    if (pairing::doIdent()) {
        std::cout << "FAIL! Device's response is below:\n";
        std::cout << pairing::pbuf->toString() << std::endl;
    }
    std::cout << "SUCCESS!" << std::endl;


    // do intent query process
    std::cout << "INTENT_Q... ";
    if (pairing::doIntentQ()) {
        std::cout << "FAIL! Device's response is below:\n";
        std::cout << pairing::pbuf->toString() << std::endl;
    }
    std::cout << "SUCCESS!" << std::endl;

    // get pairing info
    DHCPMan dhcp;

    return 0;
}