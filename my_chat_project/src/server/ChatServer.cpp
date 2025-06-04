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

        time_t now = time(NULL);
        std::vector<int> to_remove;
        for (auto& kv : last_pong) {
            int fd = kv.first;
            time_t t = kv.second;
            if (now - t > 60) {
                to_remove.push_back(fd);
            }
        }
        for (int fd : to_remove) {
            std::string user = client_usernames[fd];
            std::string oldchan = client_channels[fd];
            if (!oldchan.empty()) {
                auto& vec = channels[oldchan];
                vec.erase(std::remove(vec.begin(), vec.end(), fd), vec.end());
                std::string note = "[Server] " + user + " timed out and left " + oldchan + "\n";
                for (int other : channels[oldchan]) {
                    write(other, note.c_str(), note.size());
                }
            }
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
            client_usernames.erase(fd);
            client_channels.erase(fd);
            last_pong.erase(fd);
            close(fd);
        }
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
