cmake_minimum_required(VERSION 3.12)

set(PICO_BOARD pico_w)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(pico_sdk_import.cmake)
project(at28hc64_reader C CXX ASM)

pico_sdk_init()

add_compile_options(
    -Wall
)

add_executable(reader
    main.cpp
    src/at28hc64.cpp
)

# custom include
target_include_directories(reader PRIVATE
    ./include
)

target_link_libraries(reader
    pico_stdlib
    pico_cyw43_arch_none
    pico_time
    hardware_gpio
)

# enable usb output, disable uart output
pico_enable_stdio_usb(reader 1)
pico_enable_stdio_uart(reader 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(reader)