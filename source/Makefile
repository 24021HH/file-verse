
CXX := g++
CXXFLAGS := -Wall -Wextra -Werror -g -std=c++17

SRC_DIR := .

TEST_SRCS := fs_init.cpp main.cpp
TEST_OBJS := $(TEST_SRCS:.cpp=.o)
TEST_TARGET := ofstest

SERVER_SRCS := fs_init.cpp server.cpp request_handler.cpp main_server.cpp
SERVER_OBJS := $(SERVER_SRCS:.cpp=.o)
SERVER_TARGET := ofsserver

CLIENT_SRCS := client.cpp
CLIENT_OBJS := $(CLIENT_SRCS:.cpp=.o)
CLIENT_TARGET := ofsclient

.PHONY: all build run clean test_run server_run client_run ui_run ui_run_demo
all: build

build: $(TEST_TARGET) $(SERVER_TARGET) $(CLIENT_TARGET)
	@echo "[make] Built $(TEST_TARGET) and $(SERVER_TARGET)"

$(TEST_TARGET): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_OBJS)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(SERVER_OBJS)

$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(CLIENT_OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test_run: $(TEST_TARGET)
	@echo "[make] Running $(TEST_TARGET) (Part 1: Core Design Test)"
	./$(TEST_TARGET)

server_run: $(SERVER_TARGET)
	@echo "[make] Running $(SERVER_TARGET) (Part 2: Server Implementation)"
	./$(SERVER_TARGET) 8080

client_run: $(CLIENT_TARGET)
	@echo "[make] Running $(CLIENT_TARGET) (Interactive Client for Part 2)"
	./$(CLIENT_TARGET) 127.0.0.1 8080

ui_run:
	@echo "[make] Running Python Tkinter UI (ui/client_gui.py)"
	@python3 ../ui/client_gui.py

ui_run_demo:
	@echo "[make] Running Python Tkinter UI in demo mode"
	@python3 ../ui/client_gui.py --demo

run: test_run

clean:
	rm -f $(TEST_OBJS) $(SERVER_OBJS) $(CLIENT_OBJS) $(TEST_TARGET) $(SERVER_TARGET) $(CLIENT_TARGET)
	@echo "[make] Cleaned"
