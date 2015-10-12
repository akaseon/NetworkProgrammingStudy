
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

int UDPServerServer( short aListenPort )
{
    int                 sSocket = 0;
    int                 sClientSocket = 0;
    struct sockaddr_in  sServerAddr;
    struct sockaddr_in  sClientAddr;
    unsigned int        sClientAddrLength = 0;
    int                 sOptionVal = 0;
    int                 sReceiveLength = 0;
    char                sBuffer[MAX_BUFFER_SIZE];

    sSocket = socket( PF_INET, 
                      SOCK_DGRAM,
                      0 );
    if ( sSocket < 0 )
    {
        printf( "socket Function is failed\n" );
        return -1;
    }
    else
    {
        /* do nothing */
    }

    memset( &sServerAddr, 0x00, sizeof( sServerAddr ) );
    sServerAddr.sin_family = AF_INET;
    sServerAddr.sin_addr.s_addr = htonl( INADDR_ANY );
    sServerAddr.sin_port = htons( aListenPort );

    if ( bind( sSocket, 
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

    while ( 1 )
    {
        sClientAddrLength = sizeof( sClientAddr );

        sReceiveLength = recvfrom( sSocket, 
                                   sBuffer,
                                   MAX_BUFFER_SIZE,
                                   0,
                                   (struct sockaddr*)&sClientAddr,
                                   &sClientAddrLength );

        if ( sReceiveLength > 0 )
        {
            printf( "Client is sent data(IP:%s, Port:%d)\n",
                    inet_ntoa(sClientAddr.sin_addr),
                    ntohs(sClientAddr.sin_port) );
            printf( "Data is recvived: %s\n", sBuffer );

            if ( sendto( sSocket,
                         sBuffer,
                         sReceiveLength,
                         0,
                         (struct sockaddr*)&sClientAddr,
                         sizeof(sClientAddr) ) > 0 )
            {
                /* do nothing */
            }
            else
            {
                printf( "sendto Function is failed\n" );
            }
        }
        else
        {
            /* do nothing */
        }
    }

    return 0;
}

int main( void )
{
    UDPServerServer( PORT_NO );

    return 0;
}
