#include "CommandHandlers.h"
#include <algorithm>
#include <unistd.h>
#include <cstring>
#include <sys/epoll.h>

void handleTimeouts(std::unordered_map<int, std::string>& client_usernames,
                    std::unordered_map<int, std::string>& client_channels,
                    std::unordered_map<std::string, std::vector<int>>& channels,
                    std::unordered_map<int, time_t>& last_pong,
                    int epoll_fd,
                    int timeout_seconds) {
    time_t now = time(NULL);
    std::vector<int> to_remove;
    for (auto& kv : last_pong) {
        int fd = kv.first;
        time_t t = kv.second;
        if (now - t > timeout_seconds) {
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
}

void handleList(int client_fd, const std::unordered_map<std::string, std::vector<int>>& channels) {
    std::string list;
    for (const auto& p : channels) {
        list += p.first + " ";
    }
    if (!list.empty() && list.back() == ' ') list.pop_back();
    list += "\n";
    write(client_fd, list.c_str(), list.size());
}

void handleJoin(int client_fd, const std::string& msg, std::unordered_map<int, std::string>& client_channels,
                std::unordered_map<std::string, std::vector<int>>& channels,
                std::unordered_map<std::string, std::deque<std::string>>& history,
                const std::unordered_map<int, std::string>& client_usernames) {
    std::string channel;
    size_t end = msg.find('\n', 6);
    if (end == std::string::npos) end = msg.size();
    channel = msg.substr(6, end - 6);
    if (channel.find(' ') != std::string::npos || channel.find('\t') != std::string::npos) {
        std::string reply = "Channel names cannot contain spaces or tabs\n";
        write(client_fd, reply.c_str(), reply.size());
        return;
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
        std::string username = client_usernames.at(client_fd);
        std::string join_note = "[Server] " + username + " has joined " + channel + "\n";
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
}

void handleLeave(int client_fd, std::unordered_map<int, std::string>& client_channels,
                 std::unordered_map<std::string, std::vector<int>>& channels) {
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
}

void handleCreate(int client_fd, const std::string& msg,
                  std::unordered_map<std::string, std::vector<int>>& channels,
                  std::unordered_map<std::string, std::deque<std::string>>& history) {
    std::string channel;
    size_t end = msg.find('\n', 8);
    if (end == std::string::npos) end = msg.size();
    channel = msg.substr(8, end - 8);
    if (channel.find(' ') != std::string::npos || channel.find('\t') != std::string::npos) {
        std::string reply = "Channel names cannot contain spaces or tabs\n";
        write(client_fd, reply.c_str(), reply.size());
        return;
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
}

void handleCurrent(int client_fd,
                   const std::unordered_map<int, std::string>& client_channels) {
    std::string channel = client_channels.at(client_fd);
    if (channel.empty()) {
        std::string reply = "You're not in any channel\n";
        write(client_fd, reply.c_str(), reply.size());
    } else {
        std::string reply = "You are in " + channel + "\n";
        write(client_fd, reply.c_str(), reply.size());
    }
}

void handleRename(int client_fd, const std::string& msg,
                  std::unordered_map<int, std::string>& client_usernames) {
    std::string newname;
    size_t end = msg.find('\n', 8);
    if (end == std::string::npos) end = msg.size();
    newname = msg.substr(8, end - 8);
    if (newname.find(' ') != std::string::npos || newname.find('\t') != std::string::npos) {
        std::string reply = "Username cannot contain spaces or tabs\n";
        write(client_fd, reply.c_str(), reply.size());
        return;
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
}

void handleDelete(int client_fd, const std::string& msg,
                  std::unordered_map<std::string, std::vector<int>>& channels,
                  std::unordered_map<std::string, std::deque<std::string>>& history) {
    std::string channel;
    size_t end = msg.find('\n', 8);
    if (end == std::string::npos) end = msg.size();
    channel = msg.substr(8, end - 8);
    if (channel.find(' ') != std::string::npos || channel.find('\t') != std::string::npos) {
        std::string reply = "Channel names cannot contain spaces or tabs\n";
        write(client_fd, reply.c_str(), reply.size());
        return;
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
}

void handleAnnouncement(int client_fd, const std::string& msg,
                        const std::string& username,
                        const std::unordered_map<int, std::string>& client_usernames) {
    std::string text = msg.substr(14);
    if (!text.empty() && text.back() == '\n') text.pop_back();
    std::string note = "[Announcement] " + text + "\n";
    for (const auto& kv : client_usernames) {
        int fd = kv.first;
        write(fd, note.c_str(), note.size());
    }
}

void handleQuit(int client_fd,
                std::unordered_map<int, std::string>& client_usernames,
                std::unordered_map<int, std::string>& client_channels,
                std::unordered_map<int, time_t>& last_pong,
                int epoll_fd) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
    client_usernames.erase(client_fd);
    client_channels.erase(client_fd);
    last_pong.erase(client_fd);
    close(client_fd);
}

void handleHelp(int client_fd) {
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
        "/announcement <text> - send announcement to all users\n"
        "/ping         - ping server\n"
        "/quit         - disconnect\n";
    write(client_fd, help.c_str(), help.size());
}

void handlePing(int client_fd, std::unordered_map<int, time_t>& last_pong) {
    std::string pong = "/pong\n";
    write(client_fd, pong.c_str(), pong.size());
    last_pong[client_fd] = time(NULL);
}

void handleUsers(int client_fd, const std::string& msg,
                 const std::unordered_map<std::string, std::vector<int>>& channels,
                 const std::unordered_map<int, std::string>& client_usernames) {
    std::string channel;
    size_t end = msg.find('\n', 7);
    if (end == std::string::npos) end = msg.size();
    channel = msg.substr(7, end - 7);
    if (channels.count(channel)) {
        std::string userlist;
        for (int fd : channels.at(channel)) {
            userlist += client_usernames.at(fd) + " ";
        }
        if (!userlist.empty() && userlist.back() == ' ') userlist.pop_back();
        userlist += "\n";
        write(client_fd, userlist.c_str(), userlist.size());
    } else {
        std::string reply = "Channel " + channel + " does not exist\n";
        write(client_fd, reply.c_str(), reply.size());
    }
}
void handleMsg(int client_fd, const std::string& msg,
               const std::unordered_map<int, std::string>& client_usernames) {
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
            std::string pm = "[PM from " + client_usernames.at(client_fd) + "] " + text + "\n";
            write(target_fd, pm.c_str(), pm.size());
        }
    }
}

void handleWho(int client_fd, const std::unordered_map<int, std::string>& client_usernames) {
    std::string list;
    for (const auto& kv : client_usernames) {
        list += kv.second + " ";
    }
    if (!list.empty() && list.back() == ' ') list.pop_back();
    list += "\n";
    write(client_fd, list.c_str(), list.size());
}

void handleBroadcast(int client_fd, const std::string& msg, const std::string& username,
                     std::unordered_map<int, std::string>& client_channels,
                     std::unordered_map<std::string, std::vector<int>>& channels,
                     std::unordered_map<std::string, std::deque<std::string>>& history) {
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


void processCommand(int client_fd,
                    const std::string& msg,
                    const std::string& username,
                    std::unordered_map<int, std::string>& client_channels,
                    std::unordered_map<std::string, std::vector<int>>& channels,
                    std::unordered_map<std::string, std::deque<std::string>>& history,
                    std::unordered_map<int, std::string>& client_usernames,
                    std::unordered_map<int, time_t>& last_pong,
                    int epoll_fd) {
    if (msg == "/list\n") {
        handleList(client_fd, channels);
    } else if (msg.rfind("/create ", 0) == 0) {
        handleCreate(client_fd, msg, channels, history);
    } else if (msg.rfind("/join ", 0) == 0) {
        handleJoin(client_fd, msg, client_channels, channels, history, client_usernames);
    } else if (msg == "/leave\n") {
        handleLeave(client_fd, client_channels, channels);
    } else if (msg == "/current\n") {
        handleCurrent(client_fd, client_channels);
    } else if (msg.rfind("/rename ", 0) == 0) {
        handleRename(client_fd, msg, client_usernames);
    } else if (msg.rfind("/delete ", 0) == 0) {
        handleDelete(client_fd, msg, channels, history);
    } else if (msg.rfind("/msg ", 0) == 0) {
        handleMsg(client_fd, msg, client_usernames);
    } else if (msg == "/who\n") {
        handleWho(client_fd, client_usernames);
    } else if (msg == "/help\n") {
        handleHelp(client_fd);
    } else if (msg == "/ping\n") {
        handlePing(client_fd, last_pong);
    } else if (msg.rfind("/users ", 0) == 0) {
        handleUsers(client_fd, msg, channels, client_usernames);
    } else if (msg.rfind("/announcement ", 0) == 0) {
        handleAnnouncement(client_fd, msg, username, client_usernames);
    } else if (msg == "/quit\n") {
        handleQuit(client_fd, client_usernames, client_channels, last_pong, epoll_fd);
    } else {
        handleBroadcast(client_fd, msg, username, client_channels, channels, history);
    }
}