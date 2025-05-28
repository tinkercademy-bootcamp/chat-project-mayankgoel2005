#ifndef CHAT_SOCKETS_H
#define CHAT_SOCKETS_H

#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>


#include "../utils.h"

namespace tt::chat::net {

int create_socket();
sockaddr_in create_address(int port);
void set_socket_options(int sock, int opt);
void bind_address_to_socket(int sock, sockaddr_in &address);
void listen_on_socket(int sock, int backlog = 3);
int accept_connection(int sock, sockaddr_in &address);
void connect_to_server(int sock, sockaddr_in &server_address);

} // namespace tt::chat::net

#endif // CHAT_SOCKETS_H
