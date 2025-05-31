# ─── Makefile ────────────────────────────────────────────────────────────────

CXX       := g++
CXXFLAGS  := -std=c++23 -Wall

BUILD_DIR := build
SRC_DIR   := src

# Source files for each executable
SERVER_SRC := $(SRC_DIR)/main_server.cpp
CLIENT_SRC := $(SRC_DIR)/main_client.cpp

# Output binaries
SERVER_BIN := $(BUILD_DIR)/server
CLIENT_BIN := $(BUILD_DIR)/client

# Default target: build both server and client
all: $(SERVER_BIN) $(CLIENT_BIN)

# Rule to build server
$(SERVER_BIN): $(SERVER_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

# Rule to build client
$(CLIENT_BIN): $(CLIENT_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean