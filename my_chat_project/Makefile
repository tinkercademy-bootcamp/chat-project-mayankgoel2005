CXX       := g++
CXXFLAGS  := -std=c++23 -Wall

BUILD_DIR := build
SRC_DIR   := src

# Now pointing to the relocated files:
SERVER_SRC := $(SRC_DIR)/server/ChatServer.cpp
CLIENT_SRC := $(SRC_DIR)/client/ChatClient.cpp

# Output binaries
SERVER_BIN := $(BUILD_DIR)/server
CLIENT_BIN := $(BUILD_DIR)/client

all: $(SERVER_BIN) $(CLIENT_BIN)

$(SERVER_BIN): $(SERVER_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(CLIENT_BIN): $(CLIENT_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean