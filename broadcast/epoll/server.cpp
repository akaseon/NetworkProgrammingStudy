#include "server.h"

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

void handleListenSockEvent( void * aArg )
{
    int                 sListenSocket = 0;
    int                 sClientSocket = 0;
    struct sockaddr_in  sClientAddr;
    unsigned int        sClientAddrLength = 0;
    eventInfo         * sEventInfo;

    sEventInfo = (eventInfo*)aArg;

    sListenSocket = sEventInfo->mFD;

    sClientAddrLength = sizeof( sClientAddr );

    sClientSocket = accept( sListenSocket,
                            (struct sockaddr*)&sClientAddr,
                            &sClientAddrLength );

    if ( sClientSocket > 0 )
    {
        printf( "Client is Connected (IP:%s, Port:%d)\n",
                inet_ntoa(sClientAddr.sin_addr),
                ntohs(sClientAddr.sin_port) );

        addEvent( sEventInfo->mEpollFD,
                  sClientSocket,
                  sEventInfo->mClientList,
                  handleClientSockEvent );

        sEventInfo->mClientList->push_back( sClientSocket );
    }
    else
    {
        printf(" Accept Function is failed\n" );
        return;
    }

    return;
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

void handleClientSockEvent( void * aArg )
{
    int           sClientSock = 0;
    int           sReceiveLength = 0;
    char          sBuffer[MAX_BUFFER_SIZE];
    eventInfo   * sEventInfo = NULL;

    sEventInfo = (eventInfo*)aArg;

    sClientSock= sEventInfo->mFD;

    sReceiveLength = read( sClientSock, sBuffer, MAX_BUFFER_SIZE );
    if ( sReceiveLength == 0 )
    {
        printf( "connection is closed\n" );
        removeEvent( sEventInfo );

        return ;
    }
    else if ( sReceiveLength < 0 )
    {
        printf( "read Function is failed\n" );
        removeEvent( sEventInfo );
        return ;
    }
    else
    {
        sBuffer[sReceiveLength] = '\0';
        printf("Client is sent : %s(Size:%d)\n", sBuffer, sReceiveLength );

        sendAllClient( sBuffer,
                       sReceiveLength,
                       sClientSock,
                       *(sEventInfo->mClientList) );
    }

    return ;
}

int addEvent( int                 aEpollFd,
              int                 aSocket,
              std::list<int>    * aClientList,
              void                ( * aCallback )( void * aArg ) )
{
    struct epoll_event       sEvent;
    eventInfo              * sEventInfo = NULL;
    int                      sRc = 0;

    sEventInfo = (eventInfo*)malloc( sizeof( eventInfo ) );
    if ( sEventInfo == NULL )
    {
        printf( "malloc is failed\n");
        return -1;
    }

    sEventInfo->mEpollFD = aEpollFd;
    sEventInfo->mFD = aSocket;
    sEventInfo->mCallback = aCallback;
    sEventInfo->mClientList = aClientList;

    sEvent.events = EPOLLIN;
    sEvent.data.ptr = sEventInfo;

    sRc = epoll_ctl( aEpollFd,
                     EPOLL_CTL_ADD,
                     aSocket,
                     &sEvent );
    if ( sRc < 0 )
    {
        printf( "epoll_ctl is failed\n" );
        return -1;
    }

    return 0;
}

void removeEvent( eventInfo * aEventInfo )
{
    int     sEpollFd = 0;
    int     sSocket = 0;
    int     sRC = 0;

    sEpollFd = aEventInfo->mEpollFD;
    sSocket = aEventInfo->mFD;

    sRC = epoll_ctl( sEpollFd,
                     EPOLL_CTL_DEL,
                     sSocket,
                     NULL );

    if ( sRC < 0 )
    {
        printf( "epoll_ctl is failed\n" );
    }
}

void handleEvent( struct epoll_event    * aEvents,
                  int                     aEventCount )
{
    int           i = 0;
    eventInfo   * sEventInfo = NULL;

    for ( i = 0; i < aEventCount; i++ )
    {
        sEventInfo = (eventInfo*)aEvents[i].data.ptr;

        sEventInfo->mCallback( (void*)sEventInfo );
    }
}


int server( short aListenPort )
{
    int                 sEpollFd = 0;
    int                 sListenSocket  = 0;
    std::list<int>      sClientList;
    struct epoll_event  sEvents[MAX_EVENT];
    int                 sEventCount = 0;

    sEpollFd = epoll_create( 100 );
    if ( sEpollFd < 0 )
    {
        printf( "epoll_create is failed\n" );
        return -1;
    }

    sListenSocket = initializeListenSock( aListenPort );

    if ( sListenSocket <= 0 )
    {
        printf( "initializeListenSock is failed\n" );
        return -1;
    }

    addEvent( sEpollFd,
              sListenSocket,
              &sClientList,
              &handleListenSockEvent );

    while ( 1 )
    {
        printf( "wait event\n" );

        sEventCount = epoll_wait( sEpollFd,
                                  sEvents,
                                  MAX_EVENT,
                                  -1 );

        if ( sEventCount != 0 )
        {
            handleEvent( sEvents,
                         sEventCount );
        }
    }

    return 0;
}

int main( void )
{
    int     sRC = 0;

    sRC = server( PORT_NO );

    return sRC;
}
