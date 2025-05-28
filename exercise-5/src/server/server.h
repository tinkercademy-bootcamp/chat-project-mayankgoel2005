#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

namespace tt::chat::server {

class Server {
public:
  Server(int port);
  ~Server();
  
  void start();
  void stop();
  
private:
  int socket_fd_;
  sockaddr_in address_;
  int port_;
  bool running_;
  
  void initialize();
  void handle_connection(int client_socket);
};

} // namespace tt::chat::server

#endif // SERVER_H
