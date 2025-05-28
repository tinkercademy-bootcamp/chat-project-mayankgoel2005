#include "client.h"
#include "../net/chat-sockets.h"
#include "../utils.h"
#include <arpa/inet.h>

namespace tt::chat::client {

Client::Client(const std::string& server_ip, int port) 
    : server_ip_(server_ip), port_(port), connected_(false), socket_fd_(-1) {
  initialize();
}

Client::~Client() {
  disconnect();
}

void Client::initialize() {
  socket_fd_ = tt::chat::net::create_socket();
  
  // Set up server address
  server_address_ = tt::chat::net::create_address(port_);
  
  // Convert IP address to binary format
  auto err_code = inet_pton(AF_INET, server_ip_.c_str(), &server_address_.sin_addr);
  tt::chat::check_error(err_code <= 0, "Invalid address/ Address not supported\n");
}

bool Client::connect() {
  if (connected_) return true;
  
  try {
    tt::chat::net::connect_to_server(socket_fd_, server_address_);
    connected_ = true;
    return true;
  } catch (const std::exception& e) {
    std::cerr << "Connection error: " << e.what() << std::endl;
    return false;
  }
}

bool Client::send_message(const std::string& message) {
  if (!connected_) {
    std::cerr << "Not connected to server" << std::endl;
    return false;
  }

  auto bytes_sent = send(socket_fd_, message.c_str(), message.size(), 0);
  if (bytes_sent < 0) {
    std::cerr << "Failed to send message" << std::endl;
    return false;
  }
  
  std::cout << "Sent: " << message << "\n";
  return true;
}

std::string Client::receive_message() {
  if (!connected_) {
    std::cerr << "Not connected to server" << std::endl;
    return "";
  }

  const int kBufferSize = 1024;
  char buffer[kBufferSize] = {0};
  
  ssize_t read_size = read(socket_fd_, buffer, kBufferSize);
  
  if (read_size > 0) {
    return std::string(buffer, read_size);
  } else if (read_size == 0) {
    std::cout << "Server closed connection.\n";
    connected_ = false;
  } else {
    std::cerr << "Read error.\n";
  }
  
  return "";
}

void Client::disconnect() {
  if (socket_fd_ >= 0) {
    close(socket_fd_);
    socket_fd_ = -1;
    connected_ = false;
  }
}

} // namespace tt::chat::client
