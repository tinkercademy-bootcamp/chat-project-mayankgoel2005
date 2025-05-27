#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const int kPort = 8080;
const std::string kServerAddress = "127.0.0.1";
const int kBufferSize = 1024;

std::string getMessageFromArgs(int argc, char* argv[]) {
  std::string message = "Hello from client";
  if (argc > 1) {
    message = argv[1];
    for (int i = 2; i < argc; ++i) {
      message += " ";
      message += argv[i];
    }
  }
  return message;
}

int createSocket() {
  int my_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (my_sock < 0) {
    std::cerr << "Socket creation error\n";
    return -1;
  }
  return my_sock;
}

bool connectToServer(int my_sock, sockaddr_in& address) {
  address.sin_family = AF_INET;
  address.sin_port = htons(kPort);
  if (inet_pton(AF_INET, kServerAddress.c_str(), &address.sin_addr) <= 0) {
    std::cerr << "Invalid address/ Address not supported\n";
    return false;
  }
  if (connect(my_sock, (sockaddr*)&address, sizeof(address)) < 0) {
    std::cerr << "Connection Failed\n";
    return false;
  }
  return true;
}

void sendMessage(int my_sock, const std::string& message) {
  send(my_sock, message.c_str(), message.size() + 1, 0);
  std::cout << "Sent: " << message << "\n";
}

void receiveMessage(int my_sock) {
  char buffer[kBufferSize] = {0};
  ssize_t read_size = read(my_sock, buffer, kBufferSize);
  std::cout << "Received: " << buffer << "\n";
}

int main(int argc, char* argv[]) {
  std::string message = getMessageFromArgs(argc, argv);

  int my_sock = createSocket();
  if (my_sock < 0) return -1;

  sockaddr_in address;
  if (!connectToServer(my_sock, address)) {
    close(my_sock);
    return -1;
  }

  sendMessage(my_sock, message);
  receiveMessage(my_sock);

  close(my_sock);
  return 0;
}