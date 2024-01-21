#include "hardware/serial_interface.hpp"
#include <iostream>

extern "C" {
    #include <pico/stdlib.h>
}

void serial_recv(std::string& buf) {
    // get line from serial and store in buf
    std::cin >> buf;
    // for some reason, null char is appended to the front of buf (sometimes).
    // remove that shit so everything doesnt shatter into a bajillion pieces.
    if (buf[0] == 0)
        buf.erase(buf.begin(), buf.begin()+1);
}

void serial_send(std::string msg) {
    // send line to serial
    std::cout << msg << std::endl;
}

void serial_ack() {
    std::cout << "PICO_MOD::ACK" << std::endl;
}

void serial_clear() {
    sleep_ms(2000);
    std::cout << (char)27 << "[2J" << std::endl;
    sleep_ms(2000);
}