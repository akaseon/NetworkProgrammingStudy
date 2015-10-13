
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <string.h>

#define PORT_NO             (5432)
#define MAX_BACKLOG         (5)
#define MAX_BUFFER_SIZE     (1024)

int TCPServerHandleClient( int aClientSock )
{
    int     sReceiveLength = 0;
    char    sBuffer[MAX_BUFFER_SIZE];

    sReceiveLength = read( aClientSock, sBuffer, MAX_BUFFER_SIZE );
    if ( sReceiveLength < 0 )
    {
        printf( "read Function is failed\n" );
        return -1;
    }
    else
    {
        sBuffer[sReceiveLength] = '\0';
        printf("Client is sent : %s(Size:%d)\n", sBuffer, sReceiveLength );
        if ( write( aClientSock, sBuffer, sReceiveLength+1 ) < 0 )
        {
            printf( "write Function is failed\n" );
            return -1;
        }
        else
        {
            /* do nothing */
        }
    }

    return 0;
}

int TCPServerServer( short aListenPort )
{
    int                 sListenSocket = 0;
    int                 sClientSocket = 0;
    struct sockaddr_in  sServerAddr;
    struct sockaddr_in  sClientAddr;
    unsigned int        sClientAddrLength = 0;
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
        return -1;
    }
    else
    {
        /* do nothing */
    }

    if ( listen( sListenSocket, MAX_BACKLOG ) < 0 )
    {
        printf( "listen Function is failed\n" );
        return -1;
    }
    else
    {
        /* do nothing */
    }

    while ( 1 )
    {
        sClientAddrLength = sizeof( sClientAddr );

        sClientSocket = accept( sListenSocket, 
                                (struct sockaddr*)&sClientAddr,
                                &sClientAddrLength );

        if ( sClientSocket > 0 )
        {
            printf( "Client is Connected (IP:%s, Port:%d)\n",
                    inet_ntoa(sClientAddr.sin_addr),
                    ntohs(sClientAddr.sin_port) );

            (void)TCPServerHandleClient( sClientSocket );

            close( sClientSocket );
            sClientSocket = 0;
        }
        else
        {
            printf(" Accept Function is failed\n" );
        }
    }

    return 0;
}

int main( void )
{
    TCPServerServer( PORT_NO );

    return 0;
}
