cmake_minimum_required(VERSION 3.15)

# set C/C++ standards and options
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

project(module_pairing)

# add new executable
add_executable(modpair
    main.cpp

    # serialib courtesy of https://github.com/imabot2/serialib
    src/lib/serialib.cpp

    # serial comms and pairing
    src/serialpacket.cpp

    # networking & database
    src/networking/addressing.cpp
    src/networking/dhcpman.cpp
)
include_directories(./include)
