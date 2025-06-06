#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const int kPort = 8080;
const int kBufferSize = 1024;

int createAndSetupSocket(sockaddr_in& address) {
  int my_sock;
  int opt = 1;

  // Creating socket file descriptor
  if ((my_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cerr << "Socket creation error\n";
    return -1;
  }

  // Attaching socket to port
  if (setsockopt(my_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    std::cerr << "setsockopt error\n";
    return -1;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(kPort);

  // Bind the socket to the network address and port
  if (bind(my_sock, (sockaddr*)&address, sizeof(address)) < 0) {
    std::cerr << "bind failed\n";
    return -1;
  }

  return my_sock;
}

bool startListening(int my_sock) {
  if (listen(my_sock, 3) < 0) {
    std::cerr << "listen failed\n";
    return false;
  }
  std::cout << "Server listening on port " << kPort << "\n";
  return true;
}

int acceptConnection(int my_sock, sockaddr_in& address, socklen_t& addrlen) {
  int new_sock = accept(my_sock, (struct sockaddr*)&address, &addrlen);
  if (new_sock < 0) {
    std::cerr << "accept error\n";
    return -1;
  }
  return new_sock;
}

void handleClientCommunication(int new_sock) {
  char buffer[kBufferSize] = {0};
  // Wait for read
  ssize_t read_size = read(new_sock, buffer, kBufferSize);
  if (read_size > 0) {
    std::cout << "Received: " << buffer << "\n";
    // Send reply
    send(new_sock, buffer, read_size, 0);
    std::cout << "Echo message sent\n";
  } else {
    std::cerr << "Error reading from client\n";
  }
  close(new_sock);
}

void runServer(int my_sock, sockaddr_in& address, socklen_t& addrlen) {
  while (true) {
    int new_sock = acceptConnection(my_sock, address, addrlen);
    if (new_sock < 0) {
      close(my_sock);
      std::cerr << "Error accepting connection\n";
      return;
    }
    handleClientCommunication(new_sock);
  }
}

int main() {
  sockaddr_in address;
  socklen_t addrlen = sizeof(address);

  int my_sock = createAndSetupSocket(address);
  if (my_sock < 0) return -1;

  if (!startListening(my_sock)) {
    close(my_sock);
    return -1;
  }

  runServer(my_sock, address, addrlen);

  close(my_sock);
  return 0;
}