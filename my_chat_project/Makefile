CXX       := g++
CXXFLAGS  := -std=c++23 -Wall

BUILD_DIR := build
SRC_DIR   := src

SERVER_SRCS := $(SRC_DIR)/server/ChatServer.cpp \
               $(SRC_DIR)/server/CommandHandlers.cpp

CLIENT_SRC := $(SRC_DIR)/client/ChatClient.cpp

SERVER_BIN := $(BUILD_DIR)/server
CLIENT_BIN := $(BUILD_DIR)/client

all: $(SERVER_BIN) $(CLIENT_BIN)

$(SERVER_BIN): $(SERVER_SRCS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SERVER_SRCS) -o $@

$(CLIENT_BIN): $(CLIENT_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CLIENT_SRC) -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean