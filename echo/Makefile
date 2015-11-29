CC = gcc

TARGETS = TCPServer \
		  TCPServerProcess \
		  TCPClient \
		  UDPServer \
		  UDPClient
SRCS = TCPServer.cpp \
	   TCPServerProcess.cpp \
	   TCPClient.cpp \
	   UDPServer.cpp \
	   UDPClient.cpp 

OBJS=$(SRCS:.cpp=.o)

LFFlags = -Wall -Werror

all : $(TARGETS)

TCP : TCPServer TCPClient TCPServerProcess

UDP : UDPServer UDPClient

TCPServer : TCPServer.o TCPServer.cpp
	$(CC) $(LFFlags) -o $@ TCPServer.o

TCPServerProcess : TCPServerProcess.o TCPServerProcess.cpp
	$(CC) $(LFFlags) -o $@ TCPServerProcess.o

TCPClient : TCPClient.o TCPClient.cpp
	$(CC) $(LFFlags) -o $@ TCPClient.o

UDPServer : UDPServer.o UDPServer.cpp
	$(CC) $(LFFlags) -o $@ UDPServer.o

UDPClient :	UDPClient.o UDPClient.cpp
	$(CC) $(LFFlags) -o $@ UDPClient.o

clean:
	rm -f $(TARGETS) *.o

