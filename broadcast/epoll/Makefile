CC = g++

TARGETS = server 

SRCS = server.cpp 

OBJS=$(SRCS:.cpp=.o)

LFFlags = -Wall -Werror

all : $(TARGETS)

server : server.o server.cpp
	$(CC) $(LFFlags) -o $@ server.o -g

clean:
	rm -f $(TARGETS) *.o

