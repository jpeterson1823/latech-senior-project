#include <operation/serial/session.hpp>
#include <iostream>

extern "C" {
    #include <pico/multicore.h>
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
    std::streamsize availChars;
    bool sizeKnown = false;
    uint8_t psize;
    while (*sessionOpen) {
        // only write to pbuf if no packet is ready
        if (!*packetReady) {
            // check if data is on std::cin
            availChars = std::cin.rdbuf()->in_avail();

            // if payload size is transmitted, load into variable
            if (!sizeKnown && availChars >= 4) {
                uint8_t buf[4];
                std::cin.rdbuf()->sgetn((char*)buf, 4);
                psize = buf[3] + 4;
                sizeKnown = true;
            }

            // continue only once size is known
            else if (sizeKnown) {
                // wait until entire packet is in std::cin
                while (std::cin.rdbuf()->in_avail() < psize && *sessionOpen) {}

                // load packet into pbuf
                while(std::cin.rdbuf()->in_avail() && *sessionOpen) {
                    pbuf[pbufs] = std::cin.rdbuf()->sbumpc();
                    // make sure no buffer overflow
                    if (pbufs < SERPAC_DBUF_SIZE && pbufs < psize)
                        pbufs++;
                    else
                        break;
                }
                // clear std::cin once there are no further chars to read
                std::cin.clear();
                // reset control vars
                sizeKnown = false;
                psize = 0;
                // set packet ready
                *packetReady = true;
            }
        }
    }
}


// Class Methods
SerialSession::SerialSession()
  : timeout(5.0f), sessionActive(false) {}

void SerialSession::setTimeout(float timeout) {
    this->timeout = timeout;
}

bool SerialSession::open() {
    if (sessionActive)
        return false;

    // reset and launch core1
    multicore_launch_core1(__SerialSessionCore1Entry);

    // set session to active
    sessionActive = true;

    // push pbuf and sessionActive pointers to multicore_fifo
    multicore_fifo_push_blocking((uint32_t)(&sessionActive));
    multicore_fifo_push_blocking((uint32_t)(&packetReady));
    multicore_fifo_push_blocking((uint32_t)(&pbuf));

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