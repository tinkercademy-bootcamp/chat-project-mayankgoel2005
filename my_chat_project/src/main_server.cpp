#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>

int create_listen_socket(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(fd);
        return -1;
    }

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        perror("bind");
        close(fd);
        return -1;
    }

    if (listen(fd, 10) < 0) {
        perror("listen");
        close(fd);
        return -1;
    }

    return fd;
}

ssize_t read_client_message(int client_fd, char* buffer, size_t bufsize) {
    ssize_t total_read = 0;
    while (total_read < static_cast<ssize_t>(bufsize) - 1) {
        ssize_t n = read(client_fd, buffer + total_read, 1);
        if (n < 0) {
            perror("read");
            return -1;
        }
        if (n == 0) {
            break;
        }
        if (buffer[total_read] == '\n') {
            total_read++;
            break;
        }
        total_read++;
    }
    buffer[total_read] = '\0';
    return total_read;
}

void handle_client(int client_fd) {
    const size_t kBufSize = 1024;
    char buffer[kBufSize];
    ssize_t len = read_client_message(client_fd, buffer, kBufSize);
    if (len <= 0) {
        std::cerr << "Failed to read from client or client closed prematurely\n";
        return;
    }
    std::cout << "Received from client: " << buffer;
    ssize_t sent = write(client_fd, buffer, len);
    if (sent < 0) {
        perror("write");
    }
}

int main() {
    int listen_fd = create_listen_socket(8080);
    if (listen_fd < 0) {
        std::cerr << "Failed to create listening socket\n";
        return 1;
    }
    wait_and_accept(listen_fd);
    return 0;
}
