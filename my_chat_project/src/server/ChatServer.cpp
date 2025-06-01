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
                if (msg == "/list\n") {
                    std::string list;
                    for (const auto& p : channels) {
                        list += p.first + " ";
                    }
                    if (!list.empty() && list.back() == ' ') list.pop_back();
                    list += "\n";
                    write(client_fd, list.c_str(), list.size());
                } else if (msg.rfind("/create ", 0) == 0) {
                    std::string channel;
                    size_t end = msg.find('\n', 8);
                    if (end == std::string::npos) end = msg.size();
                    channel = msg.substr(8, end - 8);
                    if (channel.find(' ') != std::string::npos || channel.find('\t') != std::string::npos) {
                        std::string reply = "Channel names cannot contain spaces or tabs\n";
                        write(client_fd, reply.c_str(), reply.size());
                        continue;
                    }
                    if (channels.count(channel)) {
                        std::string reply = "Channel " + channel + " already exists\n";
                        write(client_fd, reply.c_str(), reply.size());
                    } else {
                        channels[channel] = {};
                        history[channel] = {};
                        std::string reply = "Channel " + channel + " created\n";
                        write(client_fd, reply.c_str(), reply.size());
                    }
                } else if (msg.rfind("/join ", 0) == 0) {
                    std::string channel;
                    size_t end = msg.find('\n', 6);
                    if (end == std::string::npos) end = msg.size();
                    channel = msg.substr(6, end - 6);
                    if (channel.find(' ') != std::string::npos || channel.find('\t') != std::string::npos) {
                        std::string reply = "Channel names cannot contain spaces or tabs\n";
                        write(client_fd, reply.c_str(), reply.size());
                        continue;
                    }
                    if (channels.count(channel)) {
                        for (const auto& line : history[channel]) {
                            write(client_fd, line.c_str(), line.size());
                        }
                        std::string prev = client_channels[client_fd];
                        if (!prev.empty()) {
                            auto& prev_vec = channels[prev];
                            prev_vec.erase(std::remove(prev_vec.begin(), prev_vec.end(), client_fd), prev_vec.end());
                        }
                        channels[channel].push_back(client_fd);
                        client_channels[client_fd] = channel;
                        std::string join_note = username + " joined " + channel + "\n";
                        history[channel].push_back(join_note);
                        if (history[channel].size() > 100) {
                            history[channel].pop_front();
                        }
                        for (int fd : channels[channel]) {
                            write(fd, join_note.c_str(), join_note.size());
                        }
                        std::string reply = "Joined " + channel + "\n";
                        write(client_fd, reply.c_str(), reply.size());
                    } else {
                        std::string reply = "Channel " + channel + " does not exist\n";
                        write(client_fd, reply.c_str(), reply.size());
                    }
                } else if (msg == "/leave\n") {
                    std::string channel = client_channels[client_fd];
                    if (channel.empty()) {
                        std::string reply = "Not in any channel\n";
                        write(client_fd, reply.c_str(), reply.size());
                    } else {
                        auto& vec = channels[channel];
                        vec.erase(std::remove(vec.begin(), vec.end(), client_fd), vec.end());
                        client_channels[client_fd] = "";
                        std::string reply = "Left " + channel + "\n";
                        write(client_fd, reply.c_str(), reply.size());
                    }
                } else if (msg == "/current\n") {
                    std::string channel = client_channels[client_fd];
                    if (channel.empty()) {
                        std::string reply = "You're not in any channel\n";
                        write(client_fd, reply.c_str(), reply.size());
                    } else {
                        std::string reply = "You are in " + channel + "\n";
                        write(client_fd, reply.c_str(), reply.size());
                    }
                } else if (msg.rfind("/rename ", 0) == 0) {
                    std::string newname;
                    size_t end = msg.find('\n', 8);
                    if (end == std::string::npos) end = msg.size();
                    newname = msg.substr(8, end - 8);
                    if (newname.find(' ') != std::string::npos || newname.find('\t') != std::string::npos) {
                        std::string reply = "Username cannot contain spaces or tabs\n";
                        write(client_fd, reply.c_str(), reply.size());
                        continue;
                    }
                    bool exists = false;
                    for (const auto& kv : client_usernames) {
                        if (kv.second == newname) { exists = true; break; }
                    }
                    if (exists) {
                        std::string reply = "Username " + newname + " is in use\n";
                        write(client_fd, reply.c_str(), reply.size());
                    } else {
                        client_usernames[client_fd] = newname;
                        std::string reply = "Username changed to " + newname + "\n";
                        write(client_fd, reply.c_str(), reply.size());
                    }
                } else if (msg.rfind("/delete ", 0) == 0) {
                    std::string channel;
                    size_t end = msg.find('\n', 8);
                    if (end == std::string::npos) end = msg.size();
                    channel = msg.substr(8, end - 8);
                    if (channel.find(' ') != std::string::npos || channel.find('\t') != std::string::npos) {
                        std::string reply = "Channel names cannot contain spaces or tabs\n";
                        write(client_fd, reply.c_str(), reply.size());
                        continue;
                    }
                    if (!channels.count(channel)) {
                        std::string reply = "Channel " + channel + " does not exist\n";
                        write(client_fd, reply.c_str(), reply.size());
                    } else if (!channels[channel].empty()) {
                        std::string reply = "Cannot delete " + channel + ": not empty\n";
                        write(client_fd, reply.c_str(), reply.size());
                    } else {
                        channels.erase(channel);
                        history.erase(channel);
                        std::string reply = "Channel " + channel + " deleted\n";
                        write(client_fd, reply.c_str(), reply.size());
                    }
                } else if (msg.rfind("/msg ", 0) == 0) {
                    size_t user_end = msg.find(' ', 5);
                    if (user_end == std::string::npos) {
                        std::string reply = "Usage: /msg <user> <text>\n";
                        write(client_fd, reply.c_str(), reply.size());
                    } else {
                        std::string target = msg.substr(5, user_end - 5);
                        std::string text = msg.substr(user_end + 1);
                        if (!text.empty() && text.back() == '\n') text.pop_back();
                        int target_fd = -1;
                        for (const auto& kv : client_usernames) {
                            if (kv.second == target) { target_fd = kv.first; break; }
                        }
                        if (target_fd < 0) {
                            std::string reply = "User " + target + " not online\n";
                            write(client_fd, reply.c_str(), reply.size());
                        } else {
                            std::string pm = "[PM from " + username + "] " + text + "\n";
                            write(target_fd, pm.c_str(), pm.size());
                        }
                    }
                } else if (msg == "/who\n") {
                    std::string list;
                    for (const auto& kv : client_usernames) {
                        list += kv.second + " ";
                    }
                    if (!list.empty() && list.back() == ' ') list.pop_back();
                    list += "\n";
                    write(client_fd, list.c_str(), list.size());
                } else if (msg == "/help\n") {
                    std::string help =
                        "/list         - list channels\n"
                        "/create <c>   - create channel c\n"
                        "/join <c>     - join channel c\n"
                        "/leave        - leave current channel\n"
                        "/current      - show current channel\n"
                        "/rename <u>   - change username to u\n"
                        "/delete <c>   - delete channel c\n"
                        "/msg <u> <t>  - private message user u\n"
                        "/who          - list all users\n"
                        "/users <c>    - list users in channel c\n"
                        "/help         - show commands\n"
                        "/ping         - ping server\n"
                        "/quit         - disconnect\n";
                    write(client_fd, help.c_str(), help.size());
                } else if (msg == "/ping\n") {
                    std::string pong = "/pong\n";
                    write(client_fd, pong.c_str(), pong.size());
                    last_pong[client_fd] = time(NULL);
                } else if (msg.rfind("/users ", 0) == 0) {
                    std::string channel;
                    size_t end = msg.find('\n', 7);
                    if (end == std::string::npos) end = msg.size();
                    channel = msg.substr(7, end - 7);
                    if (channels.count(channel)) {
                        std::string userlist;
                        for (int fd : channels[channel]) {
                            userlist += client_usernames[fd] + " ";
                        }
                        if (!userlist.empty() && userlist.back() == ' ') userlist.pop_back();
                        userlist += "\n";
                        write(client_fd, userlist.c_str(), userlist.size());
                    } else {
                        std::string reply = "Channel " + channel + " does not exist\n";
                        write(client_fd, reply.c_str(), reply.size());
                    }
                } else {
                    if (client_channels[client_fd].empty()) {
                        std::string reply = "Join a channel first\n";
                        write(client_fd, reply.c_str(), reply.size());
                    } else {
                        std::string channel = client_channels[client_fd];
                        std::string fullmsg = username + ": " + msg;
                        history[channel].push_back(fullmsg);
                        if (history[channel].size() > 100) {
                            history[channel].pop_front();
                        }
                        for (int fd : channels[channel]) {
                            if (fd != client_fd) {
                                write(fd, fullmsg.c_str(), fullmsg.size());
                            }
                        }
                    }
                }
            }
        }
    }

    close(listen_fd);
    close(epoll_fd);
    return 0;
}