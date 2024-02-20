#include "networking/neosocket.hpp"

// set initial state of flags
static bool socket::flags::initialized = false;
static bool socket::flags::recving = false;

// create state struct and set everything to init values
static socket::state_struct state {
    rhost:     nullptr,
    rport:     -1,
    data:      nullptr,
    pcb:       nullptr,

    connected: false,
    recvd:     false
};

// create/calloc all buffers
static uint8_t* socket::buffers::tmp = (uint8_t*)calloc(NEOSOCKET_BUFSIZE, sizeof(uint8_t));
static std::vector<std::string> socket::buffers::recvq = {};