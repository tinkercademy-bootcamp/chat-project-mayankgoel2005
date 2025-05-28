#include "server.h"
#include "../net/chat-sockets.h"
#include "../utils.h"

namespace tt::chat::server {

Server::Server(int port) : port_(port), running_(false), socket_fd_(-1) {
  initialize();
}

Server::~Server() {
  stop();
}

void Server::initialize() {
  socket_fd_ = tt::chat::net::create_socket();
  tt::chat::net::set_socket_options(socket_fd_, 1);
  
  // Create and bind the server address
  address_ = tt::chat::net::create_address(port_);
  address_.sin_addr.s_addr = INADDR_ANY;
  tt::chat::net::bind_address_to_socket(socket_fd_, address_);
}

void Server::start() {
  // Start listening on the socket
  tt::chat::net::listen_on_socket(socket_fd_);
  std::cout << "Server listening on port " << port_ << "\n";
  
  running_ = true;
  while (running_) {
    int client_socket = tt::chat::net::accept_connection(socket_fd_, address_);
    handle_connection(client_socket);
  }
}

void Server::stop() {
  running_ = false;
  if (socket_fd_ >= 0) {
    close(socket_fd_);
    socket_fd_ = -1;
  }
  std::cout << "Server stopped.\n";
}

void Server::handle_connection(int client_socket) {
  const int kBufferSize = 1024;
  char buffer[kBufferSize] = {0};
  ssize_t read_size = read(client_socket, buffer, kBufferSize);

  if (read_size > 0) {
    std::cout << "Received: " << buffer << "\n";
    send(client_socket, buffer, read_size, 0);
    std::cout << "Echo message sent\n";
  } else if (read_size == 0) {
    std::cout << "Client disconnected.\n";
  } else {
    std::cerr << "Read error on client socket " << client_socket << "\n";
  }
  close(client_socket);
}

} // namespace tt::chat::server
