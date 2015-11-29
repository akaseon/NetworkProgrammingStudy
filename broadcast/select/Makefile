CC = g++

TARGETS = server \
		  client 

SRCS = server.cpp \
	   client.cpp 

OBJS=$(SRCS:.cpp=.o)

LFFlags = -Wall -Werror

all : $(TARGETS)

server : server.o server.cpp
	$(CC) $(LFFlags) -o $@ server.o -g

client : client.o client.cpp
	$(CC) $(LFFlags) -o $@ client.o -g

clean:
	rm -f $(TARGETS) *.o

