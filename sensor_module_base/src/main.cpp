#include <iostream>

extern "C" {
    #include <pico/stdio.h>
}

#include "operation/kantoku.hpp"

#define BUFSIZE 1024

int main() {
    stdio_init_all();
    sleep_ms(2000);

    // create kantoku for setup, network, and pairing
    // constructor handles serial setup, network connection, pairing, and uplink creation
    Kantoku kan;

    return 0;
}