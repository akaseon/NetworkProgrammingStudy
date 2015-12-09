#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <string.h>

#include <sys/epoll.h>

#include <stdlib.h>

#include <list>

#define PORT_NO             (5432)
#define MAX_EVENT           (10)
#define MAX_BACKLOG         (5)
#define MAX_BUFFER_SIZE     (1024)

typedef struct eventInfo
{
    int                 mEpollFD;
    int                 mFD;
    void                ( * mCallback )( void * aArg );
    std::list<int>    * mClientList;
} eventInfo;

void handleListenSockEvent( void * aArg );
void handleClientSockEvent( void * aArg );

int initializeListenSock( short          aListenPort );
int sendAllClient( char           * aBuffer,
                   int              aBufferLength,
                   int              aExceptClientSocket,
                   std::list<int> & aClientList );

int addEvent( int                 aEpollFd,
              int                 aSocket,
              std::list<int>    * aClientList,
              void                ( * aCallback )( void * aArg ) );
void removeEvent( eventInfo * aEventInfo );

void handleEvent( struct epoll_event    * aEvents,
                  int                     aEventCount );

int server( short aListenPort );
