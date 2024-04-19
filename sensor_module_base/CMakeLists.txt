cmake_minimum_required(VERSION 3.12)

if (NOT DEFINED ENV{PICO_SDK_PATH})
    if(WIN32 OR MSVC)
        set(ENV{PICO_SDK_PATH} "/Users/peter/Documents/pico-dev/pico-sdk")
    elseif(UNIX)
        set(ENV{PICO_SDK_PATH} "/home/invisa/Documents/code_stuffs/pico_stuff/pico-sdk")
    endif()
endif()

#if (NOT DEFINED ENV{PICO_EXTRAS_PATH})
#    if (WIN32)
#        set(ENV{PICO_EXTRAS_PATH} "/Users/peter/Documents/pico-dev/pico-extras")
#    elseif (UNIX)
#        set(ENV{PICO_SDK_PATH} "/home/invisa/Documents/code_stuffs/pi_pico/pico-files/pico-extras")
#    endif()
#endif()

set(PICO_BOARD pico_w)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(pico_sdk_import.cmake)
project(picow_i2ceeprom C CXX ASM)
pico_sdk_init()



add_compile_options(
    -Wall
)

add_executable(main 
    src/main.cpp
    src/testing.cpp
    src/hardware/parallel_eeprom.cpp
    src/hardware/serial_interface.cpp

    src/networking/wifi.cpp
    src/networking/sockets.cpp
    src/networking/httpreqs.cpp
    src/networking/neosocket.cpp

    src/operation/kantoku.cpp
    src/operation/sensors/upa.cpp

    src/operation/serial/session.cpp
    src/operation/serial/packet.cpp
)

# custom include
target_include_directories(main PRIVATE
    ./include
)

target_link_libraries( main
    pico_time
    pico_stdlib
    pico_multicore
    pico_cyw43_arch_lwip_threadsafe_background
    hardware_gpio
    hardware_pwm
    hardware_adc
    hardware_dma
)

# enable usb output, disable uart output
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 1)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(main)
