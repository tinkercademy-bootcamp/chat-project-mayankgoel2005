# Makefile

CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++20 -g -O3
SRC_DIR := src
BUILD_DIR := build

CLIENT_SRC := $(SRC_DIR)/tcp_echo_client.cc
SERVER_SRC := $(SRC_DIR)/tcp_echo_server.cc
COMMON_SRC := $(SRC_DIR)/common.cpp

CLIENT_BIN := $(BUILD_DIR)/client
SERVER_BIN := $(BUILD_DIR)/server

.PHONY: all clean

all: $(CLIENT_BIN) $(SERVER_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(CLIENT_BIN): $(CLIENT_SRC) $(COMMON_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(SERVER_BIN): $(SERVER_SRC) $(COMMON_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD_DIR)

