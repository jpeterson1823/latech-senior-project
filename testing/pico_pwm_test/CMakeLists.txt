cmake_minimum_required(VERSION 3.12)

if (NOT DEFINED ENV{PICO_SDK_PATH})
    if(WIN32)
        set(ENV{PICO_SDK_PATH} "/Users/peter/Documents/pico-dev/pico-sdk")
    elseif(UNIX)
        set(ENV{PICO_SDK_PATH} "/home/invisa/Documents/code_stuffs/pi_pico/pico-files/pico-sdk")
    endif()
endif()

#if (NOT DEFINED ENV{PICO_EXTRAS_PATH})
#    if (WIN32)
#        set(ENV{PICO_EXTRAS_PATH} "/Users/peter/Documents/pico-dev/pico-extras")
#    elseif (UNIX)
#        set(ENV{PICO_SDK_PATH} "/home/invisa/Documents/code_stuffs/pi_pico/pico-files/pico-extras")
#    endif()
#endif()

include(pico_sdk_import.cmake)
project(picow_pwm)
set(PICO_BOARD pico_w)
pico_sdk_init()



add_compile_options(
    -Wall
)

add_executable(main 
    main.cpp
    src/LiquidCrystal_I2C.c
    src/Transmitter.cpp
    src/Receiver.cpp
)

# custom include
target_include_directories(main PRIVATE
    ./include
)

target_link_libraries( main
    pico_stdio
    pico_stdlib
    pico_cyw43_arch_none
    pico_multicore
    hardware_gpio
    hardware_pwm
    hardware_i2c
)

# enable usb output, disable uart output
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 1)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(main)
