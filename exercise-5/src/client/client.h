#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

namespace tt::chat::client {

class Client {
public:
  Client(const std::string& server_ip, int port);
  ~Client();

  bool connect();
  bool send_message(const std::string& message);
  std::string receive_message();
  void disconnect();

private:
  int socket_fd_;
  sockaddr_in server_address_;
  std::string server_ip_;
  int port_;
  bool connected_;

  void initialize();
};

} // namespace tt::chat::client

#endif // CLIENT_H
