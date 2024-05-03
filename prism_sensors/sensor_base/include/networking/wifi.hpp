#pragma once

#include <cstdint>
#include <vector>

extern "C" {
    #include <pico/cyw43_arch.h>
}


class Wifi {
private:
    static bool SetupComplete;
    static int Setup(uint32_t country, const char* ssid, const char* pass, uint32_t auth, const char* hostname, ip_addr_t* ip, ip_addr_t* mask, ip_addr_t* gw);

public:
    static int  Connect(const char* ssid, const char* pass, ip_addr_t* ip, ip_addr_t* mask, ip_addr_t* gw, const char* hostname = "PicoW");
    static void GetMacString(std::string& buf);
};
