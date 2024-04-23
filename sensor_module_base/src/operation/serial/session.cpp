#include <operation/serial/session.hpp>
#include <iostream>

extern "C" {
    #include <pico/multicore.h>
    #include <pico/stdio.h>
    #include <pico/stdlib.h>
    #include <pico/cyw43_arch.h>
}

void SerialSession::__SerialSessionCore1Entry() {
    // get flow control pointers
    bool* sessionOpen = (bool*)multicore_fifo_pop_blocking();
    bool* packetReady = (bool*)multicore_fifo_pop_blocking();

    // get packet buffer pointer and create size var
    uint8_t* pbuf = (uint8_t*)multicore_fifo_pop_blocking();
    uint8_t pbufs = 0;


    // enter session loop
    while (*sessionOpen) {
        std::cout << pbufs << std::endl;
        // only write to pbuf if no packet is ready
        if (!(*packetReady)) {
            // read char froms serial
            pbuf[pbufs++] = getchar();

            // make sure pbufs is within bounds; wrap-around if overflow
            if (pbufs > SERPAC_DBUF_SIZE)
                pbufs = 0;
            
            // continue only when size is known
            if (pbufs > 3) {
                // get characters until entire packet is read
                while (pbufs < pbuf[3]) {
                    if (*sessionOpen != true)
                        exit(0);
                    pbuf[pbufs++] = getchar();
                }

                // set packetReady
                *packetReady = true;
            }
        }
    }
}


// Class Methods
SerialSession::SerialSession()
  : timeout(5.0f), sessionActive(false), packetReady(false) {}

void SerialSession::setTimeout(float timeout) {
    this->timeout = timeout;
}

bool SerialSession::open() {
    if (sessionActive)
        return false;

    // set session to active
    sessionActive = true;

    // reset and launch core1
    multicore_launch_core1(__SerialSessionCore1Entry);

    // push pbuf and sessionActive pointers to multicore_fifo
    multicore_fifo_push_blocking((uint32_t)(&sessionActive));
    multicore_fifo_push_blocking((uint32_t)(&packetReady));
    multicore_fifo_push_blocking((uint32_t)(&pbuf));
    std::cout << "Core0 opened serial" << std::endl;

    return true;
}

bool SerialSession::close() {
    if (!sessionActive)
        return false;

    // set session to inactive
    sessionActive = false;
    return true;
}

bool SerialSession::send(SerialPacket& packet) {
    if (!sessionActive)
        return false;

    // get data pointer and length
    uint8_t* data = packet.getPayloadBytes();
    uint8_t datalen = packet.getPayloadSize();

    // send header, type, and data length in raw bytes
    std::cout << (char)packet.header[0] << (char)packet.header[1];
    std::cout << (char)packet.getType();
    std::cout << (char)datalen;

    // print every data byte out
    for (uint8_t i = 0; i < datalen; i++)
        std::cout << (char)data[i];

    return true;
}

bool SerialSession::packetAvailable() {
    if (!sessionActive)
        return false;
    return this->packetReady;
}

void SerialSession::recv(SerialPacket& packet) {
    // wait until packet is ready
    while (!packetReady) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
        sleep_ms(250);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
        sleep_ms(250);
    }

    // load data into SerialPacket and unset packetReady
    packet.fromRaw(this->pbuf);
    packetReady = false;
}

bool SerialSession::isActive() {
    return sessionActive;
}