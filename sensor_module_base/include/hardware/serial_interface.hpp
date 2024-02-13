#pragma once

#include <string>

void serial_recv(std::string& buf);
void serial_send(const char* str);
void serial_send(std::string msg);
void serial_ack();
void serial_clear();