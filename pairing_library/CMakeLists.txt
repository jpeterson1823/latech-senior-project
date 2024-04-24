cmake_minimum_required(VERSION 3.12)

project(serial_pairing)

set(CXX_STANDARD C++11)

add_executable(main 
    main.cpp
    src/lib/serialib.cpp
    src/serialpacket.cpp
    src/networking/addressing.cpp
    src/networking/dhcpman.cpp
)

target_include_directories(main PRIVATE
    ./include
)
