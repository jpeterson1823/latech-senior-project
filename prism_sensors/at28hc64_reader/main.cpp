extern "C" {
    #include <pico/stdio.h>
    #include <pico/time.h>
    #include <pico/cyw43_arch.h>
    #include <hardware/gpio.h>
}
#include <cstdint>
#include <iostream>
#include <iomanip>

#define MAX_ADDR        0x0fff
#define DATA_BUS_MASK   0x000000ffu     //gpio 0-7
#define ADDR_BUS_MASK   0x1fffff00u     //gpio 8-20
#define OE 21u
#define WE 22u
#define CE 26u

void sleep() { sleep_us(100u); }
void oeSetHi() { gpio_put(OE, 1); }
void oeSetLo() { gpio_put(OE, 0); }
void weSetHi() { gpio_put(OE, 1); }
void weSetLo() { gpio_put(OE, 0); }
void ceSetHi() { gpio_put(OE, 1); }
void ceSetLo() { gpio_put(OE, 0); }
void gpioSetup() {
    // init gpio pins
    gpio_init_mask(ADDR_BUS_MASK | DATA_BUS_MASK);
    gpio_init(WE);
    gpio_init(OE);
    gpio_init(CE);

    // set io directions
    gpio_set_dir_out_masked(ADDR_BUS_MASK);
    gpio_set_dir_in_masked(DATA_BUS_MASK);
    gpio_set_dir(WE, GPIO_OUT);
    gpio_set_dir(OE, GPIO_OUT);
    gpio_set_dir(CE, GPIO_OUT);

    // pull down all used pins
    for (uint8_t i = 0; i < 20; i++)
        gpio_pull_down(i);
    gpio_pull_down(OE);
    gpio_pull_down(WE);
    gpio_pull_down(CE);

    // set initial states
    gpio_clr_mask(ADDR_BUS_MASK);
    oeSetHi();
    weSetHi();
    ceSetLo();
}
uint8_t readByte(uint16_t addr) {
    gpio_put_masked(ADDR_BUS_MASK, (addr << 8));
    // Read condition:
    //     CE <- LOW
    //     WE <- HIGH
    //     OE <-LOW
    oeSetLo();
    sleep();
    weSetHi();
    ceSetLo();
    sleep();

    // return data bus
    return (uint8_t)(gpio_get_all() & DATA_BUS_MASK);
}
void writeByte(uint8_t byte, uint16_t addr) {
    // set address
    gpio_put_masked(ADDR_BUS_MASK, (addr << 8));
    sleep();
    // Write Condition: CE low pulse with WE low and OE high
    oeSetHi();
    sleep();

    weSetLo();
    sleep();
    weSetHi();
    sleep();

    gpio_clr_mask(ADDR_BUS_MASK);
}
void printByte(uint8_t byte) {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
}
void readEEPROM() {
    std::cout.flush();
    std::cout << std::endl;
    std::cout << "B# 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "   ";
    for (uint16_t addr = 0x0000; addr <= MAX_ADDR; addr++) {
        if (addr % 16 == 0 && addr != 0x0000) {
            std::cout << "\n   ";
            printByte(readByte(addr));
            std::cout << ' ';
        }
        else {
            printByte(readByte(addr));
            std::cout << ' ';
        }
    }
    std::cout << std::endl;
}
void cleanEEPROM() {
    std::cout << "Cleaning EEPROM..." << std::endl;
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    
    for (uint16_t addr = 0x0000; addr <= MAX_ADDR; addr++) {
        if (addr % 0x0100 == 0)
            std::cout << "   0x" << std::hex << std::setw(4) << std::setfill('0') << addr << std::endl;
        writeByte(0x00, addr);
    }

    std::cout << " DONE!";
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
}

int main() {
    stdio_init_all();
    gpioSetup();
    cyw43_arch_init();

    sleep_ms(5000);
    readEEPROM();

    return 0;
}