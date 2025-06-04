#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <deque>
#include <ctime>

void handleList(int client_fd,
                const std::unordered_map<std::string, std::vector<int>>& channels);

void handleCreate(int client_fd,
                  const std::string& msg,
                  std::unordered_map<std::string, std::vector<int>>& channels,
                  std::unordered_map<std::string, std::deque<std::string>>& history);

void handleJoin(int client_fd,
                const std::string& msg,
                std::unordered_map<int, std::string>& client_channels,
                std::unordered_map<std::string, std::vector<int>>& channels,
                std::unordered_map<std::string, std::deque<std::string>>& history,
                const std::unordered_map<int, std::string>& client_usernames);

void handleLeave(int client_fd,
                 std::unordered_map<int, std::string>& client_channels,
                 std::unordered_map<std::string, std::vector<int>>& channels);

void handleCurrent(int client_fd,
                   const std::unordered_map<int, std::string>& client_channels);

void handleRename(int client_fd,
                  const std::string& msg,
                  std::unordered_map<int, std::string>& client_usernames);

void handleDelete(int client_fd,
                  const std::string& msg,
                  std::unordered_map<std::string, std::vector<int>>& channels,
                  std::unordered_map<std::string, std::deque<std::string>>& history);

void handleMsg(int client_fd,
               const std::string& msg,
               const std::unordered_map<int, std::string>& client_usernames);

void handleWho(int client_fd,
               const std::unordered_map<int, std::string>& client_usernames);

void handleHelp(int client_fd);

void handlePing(int client_fd,
                std::unordered_map<int, time_t>& last_pong);

void handleUsers(int client_fd,
                 const std::string& msg,
                 const std::unordered_map<std::string, std::vector<int>>& channels,
                 const std::unordered_map<int, std::string>& client_usernames);

void handleAnnouncement(int client_fd,
                        const std::string& msg,
                        const std::string& username,
                        const std::unordered_map<int, std::string>& client_usernames);

void handleQuit(int client_fd,
                std::unordered_map<int, std::string>& client_usernames,
                std::unordered_map<int, std::string>& client_channels,
                std::unordered_map<int, time_t>& last_pong,
                int epoll_fd);

void handleBroadcast(int client_fd,
                     const std::string& msg,
                     const std::string& username,
                     std::unordered_map<int, std::string>& client_channels,
                     std::unordered_map<std::string, std::vector<int>>& channels,
                     std::unordered_map<std::string, std::deque<std::string>>& history);

void processCommand(int client_fd,
                    const std::string& msg,
                    const std::string& username,
                    std::unordered_map<int,std::string>& client_channels,
                    std::unordered_map<std::string,std::vector<int>>& channels,
                    std::unordered_map<std::string,std::deque<std::string>>& history,
                    std::unordered_map<int,std::string>& client_usernames,       // match non-const
                    std::unordered_map<int,time_t>& last_pong,
                    int epoll_fd);