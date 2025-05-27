# Makefile

CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++20 -g -O3 -fsanitize=address
LD_FLAGS := -fsanitize=address
SRC_DIR := src
BUILD_DIR := build

CLIENT_SRC := $(SRC_DIR)/tcp_echo_client.cc
SERVER_SRC := $(SRC_DIR)/tcp_echo_server.cc

CLIENT_BIN := $(BUILD_DIR)/client
SERVER_BIN := $(BUILD_DIR)/server

.PHONY: all clean

all: $(CLIENT_BIN) $(SERVER_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(CLIENT_BIN): $(CLIENT_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LD_FLAGS)

$(SERVER_BIN): $(SERVER_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LD_FLAGS)

clean:
	rm -rf $(BUILD_DIR)

