#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>

int create_listen_socket(int port);

ssize_t read_client_message(int client_fd, char* buffer, size_t bufsize);

int makeNonBlocking(int fd);