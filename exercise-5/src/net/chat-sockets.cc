#include "chat-sockets.h"

int tt::chat::net::create_socket() {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  tt::chat::check_error(sock < 0, "Socket creation error\n");
  return sock;
}

sockaddr_in tt::chat::net::create_address(int port) {
  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  return address;
}

void tt::chat::net::set_socket_options(int sock, int opt) {
  auto err_code = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                           &opt, sizeof(opt));
  tt::chat::check_error(err_code < 0, "setsockopt() error\n");
}

void tt::chat::net::bind_address_to_socket(int sock, sockaddr_in &address) {
  auto err_code = bind(sock, (sockaddr *)&address, sizeof(address));
  tt::chat::check_error(err_code < 0, "bind failed\n");
}

void tt::chat::net::listen_on_socket(int sock, int backlog) {
  auto err_code = listen(sock, backlog);
  tt::chat::check_error(err_code < 0, "listen failed\n");
}

int tt::chat::net::accept_connection(int sock, sockaddr_in &address) {
  socklen_t address_size = sizeof(address);
  int accepted_socket = accept(sock, (sockaddr *)&address, &address_size);
  tt::chat::check_error(accepted_socket < 0, "Accept error\n");
  return accepted_socket;
}

void tt::chat::net::connect_to_server(int sock, sockaddr_in &server_address) {
  auto err_code = connect(sock, (sockaddr *)&server_address, sizeof(server_address));
  tt::chat::check_error(err_code < 0, "Connection Failed.\n");
}
