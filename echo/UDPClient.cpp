
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <string.h>

#define IPADDRESS           (char*)("127.0.0.1")
#define PORT_NO             (5432)

#define MAX_BUFFER_SIZE     (1024)

int UDPClient( char     * aIPAddress,
               short      aPort,
               void     * aMessage,
               int        aMessageLength )
{
    int                     sSocket = 0;
    struct sockaddr_in      sServerAddress;
    unsigned int            sServerAddressLength = 0;
    char                    sBuffer[MAX_BUFFER_SIZE];
    int                     sBufferLength = 0;

    sSocket = socket( PF_INET, SOCK_DGRAM, 0 );
    if ( sSocket < 0 )
    {
        printf( "socket function is failed\n" );
        return -1;
    }

    memset( &sServerAddress, 0x00, sizeof(sServerAddress) );
    sServerAddress.sin_family = AF_INET;
    sServerAddress.sin_addr.s_addr = inet_addr( aIPAddress );
    sServerAddress.sin_port = htons( aPort );

    if ( sendto( sSocket, 
                 aMessage, 
                 aMessageLength,
                 0,
                 (struct sockaddr*)&sServerAddress,
                 sizeof(sServerAddress) )
         > 0 )
    {
        sServerAddressLength = sizeof(sServerAddress);

        if ( recvfrom( sSocket,
                       (void*)sBuffer,
                       MAX_BUFFER_SIZE,
                       0,
                       (struct sockaddr*)&sServerAddress,
                       &sServerAddressLength ) < 0 )
        {
            printf( "recvFrom function is failed\n" );
        }
        else
        {
            printf( "Server is sent data(IP:%s, Port:%d)\n",
                    inet_ntoa(sServerAddress.sin_addr),
                    ntohs(sServerAddress.sin_port) );
            printf( "Echo : %s\n", sBuffer );
        }
    }
    else
    {
        printf( "sendto function is failed\n" );
    }

    close( sSocket );
    sSocket = 0;

    return 0;
}

int main( void )
{
    char    sBuffer[MAX_BUFFER_SIZE];
    int     sBufferLength = 0;

    while ( 1 )
    {
        scanf( "%s", sBuffer );
        sBufferLength = strlen( sBuffer );

        if ( (sBuffer[0] == 'q') && (sBufferLength == 1) )
        {
            break;
        }
        else
        {
            /* do nothing */
        }

        if ( UDPClient( IPADDRESS,
                        PORT_NO,
                        sBuffer,
                        sBufferLength+1 ) == -1 )
        {
            break;
        }
        else
        {
            /* do nothing */
        }
    }

    return 0;
}
