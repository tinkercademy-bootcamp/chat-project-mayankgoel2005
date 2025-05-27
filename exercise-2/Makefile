CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++20 -g -O3
SRC_DIR := src
SRC_EX1_DIR := src_ex1
BUILD_DIR := build

CLIENT_SRC := $(SRC_DIR)/tcp_echo_client.cc
SERVER_SRC := $(SRC_DIR)/tcp_echo_server.cc

CLIENT_EX1_SRC := $(SRC_EX1_DIR)/tcp_echo_client.cc
SERVER_EX1_SRC := $(SRC_EX1_DIR)/tcp_echo_server.cc

CLIENT_BIN := $(BUILD_DIR)/client
SERVER_BIN := $(BUILD_DIR)/server
CLIENT_EX1_BIN := $(BUILD_DIR)/client_ex1
SERVER_EX1_BIN := $(BUILD_DIR)/server_ex1

.PHONY: all clean

all: $(CLIENT_BIN) $(SERVER_BIN) $(CLIENT_EX1_BIN) $(SERVER_EX1_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(CLIENT_BIN): $(CLIENT_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(SERVER_BIN): $(SERVER_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(CLIENT_EX1_BIN): $(CLIENT_EX1_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(SERVER_EX1_BIN): $(SERVER_EX1_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)
