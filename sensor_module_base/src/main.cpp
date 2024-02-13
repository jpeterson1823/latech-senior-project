#include <iostream>

extern "C" {
    #include <pico/stdio.h>
}

#include "operation/kantoku.hpp"

#define BUFSIZE 1024

int main() {
    stdio_init_all();
    sleep_ms(2000);

    Kantoku kan;



    return 0;
}