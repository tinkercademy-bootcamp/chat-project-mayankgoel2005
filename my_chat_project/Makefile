CXX       := g++
CXXFLAGS  := -std=c++23 -Wall

BUILD_DIR := build
SRC_DIR   := src

# Define the two executables and their sources
SERVER_SRC := \$(SRC_DIR)/main_server.cpp
CLIENT_SRC := \$(SRC_DIR)/main_client.cpp

SERVER_BIN := \$(BUILD_DIR)/server
CLIENT_BIN := \$(BUILD_DIR)/client

all: \$(SERVER_BIN) \$(CLIENT_BIN)

\$(BUILD_DIR)/server: \$(SERVER_SRC)
	@mkdir -p \$(BUILD_DIR)
	\$(CXX) \$(CXXFLAGS) \$< -o \$@

\$(BUILD_DIR)/client: \$(CLIENT_SRC)
	@mkdir -p \$(BUILD_DIR)
	\$(CXX) \$(CXXFLAGS) \$< -o \$@

clean:
	rm -rf \$(BUILD_DIR)

.PHONY: all clean