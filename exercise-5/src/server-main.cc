#include <iostream>
#include "server/server.h"

int main() {
  try {
    const int kPort = 8080;
    
    // Create and start the server
    tt::chat::server::Server server(kPort);
    server.start();
    
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
