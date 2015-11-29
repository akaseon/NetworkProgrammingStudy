#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <string.h>

#include <sys/select.h>

#include <list>

#define PORT_NO             (5432)
#define MAX_BACKLOG         (5)
#define MAX_BUFFER_SIZE     (1024)

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
    if ( sReceiveLength <= 0 )
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

int server( short aListenPort )
{
    int             sListenSocket  = 0;
    int             sClientSocket = 0;
    int             sMaxFd = 0;
    std::list<int>  sClientList;
    fd_set          sReadSet;
    fd_set          sAllSet;

    FD_ZERO( &sReadSet );
    FD_ZERO( &sAllSet );

    sListenSocket = initializeListenSock( aListenPort );

    if ( sListenSocket <= 0 )
    {
        printf( "initializeListenSock is failed\n" );
        return -1;
    }

    sMaxFd = sListenSocket;
    FD_SET( sListenSocket,
            &sAllSet );

    while ( 1 )
    {
        printf( "wait event\n" );

        sReadSet = sAllSet;

        select( sMaxFd + 1, 
                &sReadSet,
                NULL,   /* write */
                NULL,   /* error */
                NULL    /* timeout */ );

        /* handle listen socket event */
        if ( FD_ISSET( sListenSocket,
                       &sReadSet ) )
        {
            sClientSocket = handleListenSockEvent( sListenSocket );
            if ( sClientSocket <= 0 )
            {
                printf( "handleListenSockEvent is failed\n" );
            }
            else
            {
                FD_SET( sClientSocket,
                        &sAllSet );
                sClientList.push_back( sClientSocket );

                if ( sClientSocket > sMaxFd )
                {
                    sMaxFd = sClientSocket;
                }

                printf(" client fd %d, maxfd %d\n", sClientSocket, sMaxFd );

                sClientSocket = 0;
            }
        }

        /* handle client event */
        for ( std::list<int>::iterator sIter = sClientList.begin();
              sIter != sClientList.end();
              ++sIter )
        {
            sClientSocket = *sIter;

            if ( FD_ISSET( sClientSocket,
                           &sReadSet ) )
            {
                printf( "handle client event\n" );

                if ( handleClientSockEvent( sClientSocket,
                                            sClientList )
                     < 0 )
                {
                    FD_CLR( sClientSocket,
                            &sAllSet );
                    sClientList.remove( sClientSocket );
                    close( sClientSocket );
                    break;
                }
            }

            sClientSocket = 0;
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
