#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <string>
#include <cstring>

int connect_to_server(const char* ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        close(sock);
        return -1;
    }

    if (connect(sock, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0) {
        perror("connect");
        close(sock);
        return -1;
    }

    return sock;
}

bool send_message(int sock, const std::string& msg) {
    ssize_t total = 0;
    const char* data = msg.c_str();
    size_t len = msg.size();
    while (total < static_cast<ssize_t>(len)) {
        ssize_t n = write(sock, data + total, len - total);
        if (n < 0) {
            perror("write");
            return false;
        }
        total += n;
    }
    return true;
}

ssize_t read_server_message(int sock, char* buffer, size_t bufsize) {
    ssize_t total_read = 0;
    while (total_read < static_cast<ssize_t>(bufsize) - 1) {
        ssize_t n = read(sock, buffer + total_read, 1);
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

int main() {
    int sock = connect_to_server("127.0.0.1", 8080);
    if (sock < 0) {
        std::cerr << "Connection to server failed\n";
        return 1;
    }

    std::cout << "Connected to server 127.0.0.1:8080\n";
    std::cout << "Type messages and press Enter to send. Type 'quit' to exit.\n";

    const size_t kBufSize = 1024;
    char recvbuf[kBufSize];
    char sendbuf[kBufSize];

    while (true) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sock, &readfds);

        int maxfd = (sock > STDIN_FILENO ? sock : STDIN_FILENO) + 1;
        int ready = select(maxfd, &readfds, nullptr, nullptr, nullptr);
        if (ready < 0) {
            if (errno == EINTR) continue;
            perror("select");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            if (!fgets(sendbuf, kBufSize, stdin)) {
                std::cout << "Input closed, exiting.\n";
                break;
            }
            if (std::strcmp(sendbuf, "quit\n") == 0) {
                std::cout << "Quit command received, exiting.\n";
                break;
            }
            size_t len = std::strlen(sendbuf);
            if (len == 0 || sendbuf[len - 1] != '\n') {
                sendbuf[len] = '\n';
                sendbuf[len + 1] = '\0';
            }
            if (!send_message(sock, std::string(sendbuf))) {
                std::cerr << "Failed to send message\n";
                break;
            }
        }

        if (FD_ISSET(sock, &readfds)) {
            ssize_t n = read_server_message(sock, recvbuf, kBufSize);
            if (n <= 0) {
                std::cerr << "Server closed connection or read error\n";
                break;
            }
            std::cout << "Server: " << recvbuf;
        }
    }

    close(sock);
    return 0;
}
