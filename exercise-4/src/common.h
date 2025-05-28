#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>

// Function declarations
void check_error(bool test, const std::string &error_message);
int create_socket();
sockaddr_in create_address(const std::string &ip, int port);

#endif // COMMON_H
