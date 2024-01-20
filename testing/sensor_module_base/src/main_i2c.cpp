extern "C" {
    #include <pico/stdlib.h>
    #include <hardware/gpio.h>
    #include <hardware/i2c.h>
}

#include <iostream>
#include "eeprom.hpp"
#include "serial_interface.hpp"

bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

void bus_scan() {
    printf("\nI2C Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
 
    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }
 
        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.
 
        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (reserved_addr(addr))
            ret = PICO_ERROR_GENERIC;
        else
            ret = i2c_read_blocking(i2c_default, addr, &rxdata, 1, false);
 
        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
    printf("Done.\n");
}

int main() {
    // init serial
    stdio_init_all();

    sleep_ms(1000);

    // initialize i2c @ 100kHz

    i2c_init(i2c_default, 100'000);

    // setup gpio pins for i2c
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    // set up built-in led
    //gpio_init(CYW43_WL_GPIO_LED_PIN);
    //gpio_set_dir(CYW43_WL_GPIO_LED_PIN, GPIO_OUT);
    //gpio_put(CYW43_WL_GPIO_LED_PIN, 0);

    // receive cmd from usb serial
    //serial_clear();
    //serial_ack();
    std::string buf;
    serial_recv(buf);

    std::cout << buf << std::endl;

    // determine command: save/give data
    if (buf.compare("PICO_MOD::SET") == 0) {
        // send ack and receive data to store
        serial_ack();
        buf = "";
        serial_recv(buf);

        // write data to eeprom at addr 0
        eeprom_write_str(0, buf.c_str());

        //illuminate built-in LED
        //gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    }
    else if (buf.compare("PICO_MOD::GET") == 0) {
        // send ack
        serial_ack();

        // read data from eeprom
        buf = "";
        eeprom_read_str(0, buf);

        // send data
        serial_send(buf);
        gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    }
    else {
        serial_send("PICO_MOD::UNKNOWN_CMD. RECV:");
        serial_send(buf);
    }
    
    return 0;
}