#include <iostream>
#include <vector>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <fcntl.h>
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

int makeNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        perror("fcntl(F_GETFL)");
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("fcntl(F_SETFL)");
        return -1;
    }
    return 0;
}

static constexpr int kPort      = 8080;
static constexpr int kMaxEvents = 10;

int main() {
    int listen_fd = create_listen_socket(kPort);
    if (listen_fd < 0) {
        std::cerr << "Failed to set up listening socket\n";
        return 1;
    }

    if (makeNonBlocking(listen_fd) < 0) {
        close(listen_fd);
        return 1;
    }

    std::cout << "Server listening on port " << kPort << "\n";

    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1");
        close(listen_fd);
        return 1;
    }

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev) < 0) {
        perror("epoll_ctl: listen_fd");
        close(listen_fd);
        close(epoll_fd);
        return 1;
    }

    std::vector<epoll_event> events(kMaxEvents);
    while (true) {
        int n = epoll_wait(epoll_fd, events.data(), kMaxEvents, -1);
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < n; i++) {
            int event_fd = events[i].data.fd;

            if (event_fd == listen_fd) {
                while (true) {
                    sockaddr_in client_addr{};
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(
                        listen_fd,
                        reinterpret_cast<sockaddr*>(&client_addr),
                        &client_len
                    );
                    if (client_fd < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            break;
                        } else {
                            perror("accept");
                            break;
                        }
                    }

                    if (makeNonBlocking(client_fd) < 0) {
                        close(client_fd);
                        continue;
                    }

                    epoll_event client_ev{};
                    client_ev.events = EPOLLIN | EPOLLET;
                    client_ev.data.fd = client_fd;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev) < 0) {
                        perror("epoll_ctl: client_fd");
                        close(client_fd);
                        continue;
                    }

                    char ipbuf[INET_ADDRSTRLEN];
                    inet_ntop(
                        AF_INET,
                        &client_addr.sin_addr,
                        ipbuf,
                        sizeof(ipbuf)
                    );
                    std::cout << "Accepted connection from "
                              << ipbuf << ":"
                              << ntohs(client_addr.sin_port) << "\n";
                }
            }
            else {
                int client_fd = event_fd;
                handle_client(client_fd);

                if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr) < 0) {
                    perror("epoll_ctl: DEL client_fd");
                }
                close(client_fd);
                std::cout << "Client disconnected (fd=" << client_fd << ")\n";
            }
        }
    }

    close(listen_fd);
    close(epoll_fd);
    return 0;
}
