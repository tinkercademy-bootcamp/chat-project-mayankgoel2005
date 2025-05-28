#include <iostream>
#include <string>
#include "client/client.h"

std::string read_args(int argc, char *argv[]) {
  std::string message = "Hello from client";
  if (argc == 1) {
    std::cout << "Usage: " << argv[0] << " <message>\n";
    exit(EXIT_FAILURE);
  }
  if (argc > 1) {
    message = argv[1];
  }
  return message;
}

int main(int argc, char *argv[]) {
  try {
    const int kPort = 8080;
    const std::string kServerAddress = "127.0.0.1";

    // Parse command line arguments
    std::string message = read_args(argc, argv);

    // Create client and connect to server
    tt::chat::client::Client client(kServerAddress, kPort);

    if (!client.connect()) {
      std::cerr << "Failed to connect to server" << std::endl;
      return 1;
    }

    // Send message and receive response
    client.send_message(message);
    std::string response = client.receive_message();

    if (!response.empty()) {
      std::cout << "Received: " << response << std::endl;
    }

    // Clean up
    client.disconnect();

    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
