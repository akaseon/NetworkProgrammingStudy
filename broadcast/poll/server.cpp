#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <string.h>

#include <poll.h>

#include <list>

#define PORT_NO             (5432)
#define MAX_EVENT           (1024)
#define MAX_BACKLOG         (5)
#define MAX_BUFFER_SIZE     (1024)

typedef struct pollObj
{
    int         mObserveCount;
    pollfd      mObserveArray[MAX_EVENT];
} pollObj;

int initializeListenSock( short          aListenPort )
{
    int                 sListenSocket = 0;
    struct sockaddr_in  sServerAddr;
    int                 sOptionVal = 0;

    sListenSocket = socket( PF_INET, 
                            SOCK_STREAM, 
                            0 );
    if ( sListenSocket < 0 )
    {
        printf( "socket Function is failed\n" );
        return -1;
    }
    else
    {
        /* do nothing */
    }

    /* REUSEADDR SET */
    sOptionVal = 1;
    if ( setsockopt( sListenSocket,
                     SOL_SOCKET,
                     SO_REUSEADDR,
                     (char*)&sOptionVal,
                     (int)sizeof(sOptionVal) ) < 0 )
    {
        printf( "setSockOpt Function is failed\n" );
    }
    else
    {
        /* do nothing */
    }

    memset( &sServerAddr, 0x00, sizeof( sServerAddr ) );
    sServerAddr.sin_family = AF_INET;
    sServerAddr.sin_addr.s_addr = htonl( INADDR_ANY );
    sServerAddr.sin_port = htons( aListenPort );

    if ( bind( sListenSocket, 
               (struct sockaddr*)&sServerAddr,
               sizeof(sServerAddr) ) < 0 )
    {
        printf( "bind Function is failed\n" );
        close( sListenSocket );
        return -1;
    }
    else
    {
        /* do nothing */
    }

    if ( listen( sListenSocket, MAX_BACKLOG ) < 0 )
    {
        printf( "listen Function is failed\n" );
        close( sListenSocket );
        return -1;
    }
    else
    {
        /* do nothing */
    }

    return sListenSocket;
}

int handleListenSockEvent( int      aListenSock )
                           
{
    int                 sClientSocket = 0;
    struct sockaddr_in  sClientAddr;
    unsigned int        sClientAddrLength = 0;

    sClientAddrLength = sizeof( sClientAddr );

    sClientSocket = accept( aListenSock,
                            (struct sockaddr*)&sClientAddr,
                            &sClientAddrLength );

    if ( sClientSocket > 0 )
    {
        printf( "Client is Connected (IP:%s, Port:%d)\n",
                inet_ntoa(sClientAddr.sin_addr),
                ntohs(sClientAddr.sin_port) );
    }
    else
    {
        printf(" Accept Function is failed\n" );
        return -1;
    }

    return sClientSocket;
}

int sendAllClient( char           * aBuffer,
                   int              aBufferLength,
                   int              aExceptClientSocket,
                   std::list<int> & aClientList )
{
    int     sClientSocket = 0;

    for ( std::list<int>::iterator sIter = aClientList.begin();
          sIter != aClientList.end();
          ++sIter )
    {
        sClientSocket = *sIter;

        if ( sClientSocket != aExceptClientSocket )
        {
            if ( write( sClientSocket, aBuffer, aBufferLength+1 ) < 0 )
            {
                printf( "write Function(client %d) is failed\n", sClientSocket );
            }
        }
    }

    return 0;
}
                   
int handleClientSockEvent( int               aClientSock,
                           std::list<int>  & aClientList )
{
    int     sReceiveLength = 0;
    char    sBuffer[MAX_BUFFER_SIZE];

    sReceiveLength = read( aClientSock, sBuffer, MAX_BUFFER_SIZE );
    if ( sReceiveLength == 0 )
    {
        printf( "connection is closed\n" );
        return -1;
    }
    else if ( sReceiveLength < 0 )
    {
        printf( "read Function is failed\n" );
        return -1;
    }
    else
    {
        sBuffer[sReceiveLength] = '\0';
        printf("Client is sent : %s(Size:%d)\n", sBuffer, sReceiveLength );

        sendAllClient( sBuffer,
                       sReceiveLength,
                       aClientSock,
                       aClientList );
    }

    return 0;
}

void setObserveEvent( pollfd *  aPollfd,
                      int       aFd,
                      short     aEvent )
{
    aPollfd->fd = aFd;
    aPollfd->events = aEvent;
}

void addObserveObjToPoll( pollObj   * aPollObj,
                          int         aFd,
                          short       aEvent )
{
    int       sObserveCount = 0;

    sObserveCount = aPollObj->mObserveCount;

    setObserveEvent( &(aPollObj->mObserveArray[sObserveCount]),
            aFd,
            aEvent );
    aPollObj->mObserveCount++;
}

void removeObserveObjFromPoll( pollObj * aPollObj,
                               int       aFd )
{
    pollfd  * sPollfd = NULL;
    int       sObserveCount = 0;
    int       i = 0;

    sObserveCount = aPollObj->mObserveCount;

    for ( i = 0; i < sObserveCount; i++ )
    {
        sPollfd = &(aPollObj->mObserveArray[i]);
        if ( sPollfd->fd == aFd )
        {
            aPollObj->mObserveArray[i] = 
                aPollObj->mObserveArray[sObserveCount-1];
            aPollObj->mObserveCount--;
            break;
        }
    }
}

void handleEvent( pollObj         * aPollObj,
                  std::list<int>  & aClientList )
{
    int       i = 0;
    int       sObserveCount = 0;
    pollfd  * sPollfd = NULL;
    int       sSocket = 0;

    sPollfd = aPollObj->mObserveArray;
    sObserveCount = aPollObj->mObserveCount;

    for ( i = 0; i < sObserveCount; i++ )
    {
        if ( ( sPollfd[i].revents & POLLIN ) == POLLIN )
        {
            // handle Input event
            if ( i == 0 )
            {
                // listen socket
                sSocket = handleListenSockEvent( sPollfd[i].fd );
                if ( sSocket <= 0 )
                {
                    printf( "handleListenSockEvent is failed\n" );
                }
                else
                {
                    addObserveObjToPoll( aPollObj,
                                         sSocket,
                                         POLLIN | POLLERR );

                    aClientList.push_back( sSocket );
                    
                    sSocket = 0;
                }
            }
            else
            {
                // client socket
                sSocket = sPollfd[i].fd;

                if ( handleClientSockEvent( sSocket,
                                            aClientList )
                     < 0 )
                {
                    close( sSocket );
                    removeObserveObjFromPoll( aPollObj,
                                              sSocket );
                    aClientList.remove( sSocket );
                }
            }
        }
        else if ( ( sPollfd[i].revents & POLLERR ) == POLLERR )
        {
            // handle error event
            sSocket = sPollfd[i].fd;
            close( sSocket );
            removeObserveObjFromPoll( aPollObj,
                                      sSocket );
            aClientList.remove( sSocket );
        }
    }    
}

int server( short aListenPort )
{
    int             sListenSocket  = 0;
    pollObj         sPollObj;
    std::list<int>  sClientList;

    sListenSocket = initializeListenSock( aListenPort );

    if ( sListenSocket <= 0 )
    {
        printf( "initializeListenSock is failed\n" );
        return -1;
    }

    addObserveObjToPoll( &sPollObj,
                         sListenSocket,
                         POLLIN | POLLERR );
    while ( 1 )
    {
        printf( "wait event\n" );

        poll( sPollObj.mObserveArray,
              sPollObj.mObserveCount,
              -1 );

        handleEvent( &sPollObj,
                     sClientList );
    }

    return 0;
}

int main( void )
{
    int     sRC = 0;

    sRC = server( PORT_NO );

    return sRC;
}
