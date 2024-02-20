#include <iostream>

extern "C" {
    #include <pico/stdio.h>
}

#include "operation/kantoku.hpp"
#include "networking/neosocket.hpp"

#define BUFSIZE 1024

int main() {
    stdio_init_all();
    sleep_ms(2000);

    // create kantoku for setup, network, and pairing
    // constructor handles serial setup, network connection, pairing, and uplink creation
    Kantoku kan;
    
    // make sure module paired to controller
    if (!kan.attemptPair()) {
        std::cout << "FAILED TO PAIR TO CONTROLLER!" << std::endl;
        exit(1);
    }

    std::cout << socket::popRecvq() << std::endl;

    std::cout << "REACHED END OF MAIN!" << std::endl;

    return 0;
}