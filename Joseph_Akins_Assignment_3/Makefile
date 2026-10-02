# Makefile for EECS 348 Assignment 3 - CEO Email Priority Queue
# Builds both versions of the program: chat and claude
#
#   make            - build both programs
#   make chat       - build only chat
#   make claude     - build only claude
#   make run-chat   - build and run chat with the test file
#   make run-claude - build and run claude with the test file
#   make clean      - remove the built programs

CXX = g++
CXXFLAGS = -std=c++11 -Wall
TEST = test.txt

all: chat claude

chat: chat.cpp
	$(CXX) $(CXXFLAGS) chat.cpp -o chat

claude: claude.cpp
	$(CXX) $(CXXFLAGS) claude.cpp -o claude

run-chat: chat
	./chat $(TEST)

run-claude: claude
	./claude $(TEST)

clean:
	rm -f chat claude chat.exe claude.exe

.PHONY: all run-chat run-claude clean
