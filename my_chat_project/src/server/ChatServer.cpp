#include <iostream>
#include <vector>
#include <unordered_map>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <deque>
#include <ctime>
#include <algorithm>
#include "CommandHandlers.h"
#include "SocketUtils.h"


static constexpr int kPort      = 8080;
static constexpr int kMaxEvents = 10;
static constexpr int kBufSize   = 1024;

int main() {
    std::unordered_map<int, std::string> client_usernames;
    std::unordered_map<int, std::string> client_channels;
    std::unordered_map<std::string, std::vector<int>> channels;
    std::unordered_map<std::string, std::deque<std::string>> history;
    std::unordered_map<int, time_t> last_pong;

    int listen_fd, epoll_fd;
    if (!setupListener(kPort, listen_fd, epoll_fd)) {
        return 1;
    }

    std::vector<epoll_event> events(kMaxEvents);
    while (true) {
        int n = epoll_wait(epoll_fd, events.data(), kMaxEvents, -1);

        handleTimeouts(client_usernames, client_channels, channels, last_pong, epoll_fd, 60);

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

                    client_usernames[client_fd] = "";
                    epoll_event client_ev{};
                    client_ev.events = EPOLLIN;
                    client_ev.data.fd = client_fd;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev) < 0) {
                        perror("epoll_ctl: client_fd");
                        client_usernames.erase(client_fd);
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
                              << ipbuf << ":" << ntohs(client_addr.sin_port)
                              << " (fd=" << client_fd << ")\n";
                    std::cout << "  Waiting for username...\n";
                }
            }
            else {
                int client_fd = event_fd;
                char buffer[kBufSize];
                ssize_t len = read_client_message(client_fd, buffer, kBufSize);

                if (len <= 0) {
                    if (len < 0) {
                        std::cerr << "Error reading from client_fd " 
                                  << client_fd << "\n";
                    } else {
                        std::cout << "Client closed connection (fd=" 
                                  << client_fd << ")\n";
                    }
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
                    client_usernames.erase(client_fd);
                    close(client_fd);
                    continue;
                }

                std::string msg(buffer);

                last_pong[client_fd] = time(NULL);

                if (client_usernames[client_fd].empty()) {
                    if (!msg.empty() && msg.back() == '\n') {
                        msg.pop_back();
                    }
                    if (msg.find(' ') != std::string::npos || msg.find('\t') != std::string::npos) {
                        std::string reply = "Username cannot contain spaces or tabs\n";
                        write(client_fd, reply.c_str(), reply.size());
                        continue;
                    }
                    bool exists = false;
                    for (const auto& kv : client_usernames) {
                        if (kv.second == msg) { exists = true; break; }
                    }
                    if (exists) {
                        std::string reply = "Username already exists\n";
                        write(client_fd, reply.c_str(), reply.size());
                        continue;
                    }
                    client_usernames[client_fd] = msg;
                    std::cout << "Registered username '" << msg 
                              << "' for fd=" << client_fd << "\n";
                    std::string welcome = "Welcome, " + msg + "\n";
                    write(client_fd, welcome.c_str(), welcome.size());
                    continue;
                }

                const std::string& username = client_usernames[client_fd];
                std::cout << username << ": " << msg;
                processCommand(client_fd,
                   msg,
                   username,
                   client_channels,
                   channels,
                   history,
                   client_usernames,
                   last_pong,
                   epoll_fd);
            }
        }
    }

    close(listen_fd);
    close(epoll_fd);
    return 0;
}
