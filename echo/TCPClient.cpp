
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

int TCPClientHandle( int    aSocket,
                     void * aMessage,
                     int    aMessageLength )
{
    int     sWriteLength = 0;
    int     sReadLength = 0;
    char    sBuffer[MAX_BUFFER_SIZE];

    sWriteLength = write( aSocket,
                          aMessage,
                          aMessageLength );
    if ( sWriteLength != aMessageLength )
    {
        printf( "write function is failed(write byte:%d, expected byte:%d)\n", sWriteLength, aMessageLength );

        return -1;
    }

    sReadLength = read( aSocket,
                        sBuffer,
                        MAX_BUFFER_SIZE );
    if ( sReadLength > 0 )
    {
        sBuffer[sReadLength] = '\0';
        printf("Recv Message : %s\n", sBuffer );
    }
    else
    {
        printf( "read function is failed\n" );
    }

    return 0;
}
int TCPClient( char     * aIPAddress,
               short      aPort,
               void     * aMessage,
               int        aMessageLength )
{
    int                     sSocket = 0;
    struct sockaddr_in      sServerAddress;

    sSocket = socket( PF_INET, SOCK_STREAM, 0 );
    if ( sSocket < 0 )
    {
        printf( "socket function is failed\n" );
        return -1;
    }

    memset( &sServerAddress, 0x00, sizeof(sServerAddress) );
    sServerAddress.sin_family = AF_INET;
    sServerAddress.sin_addr.s_addr = inet_addr( aIPAddress );
    sServerAddress.sin_port = htons( aPort );

    if ( connect( sSocket, 
                  (struct sockaddr*)&sServerAddress, 
                  sizeof(sServerAddress) ) < 0 )
    {
        printf( "connect function is failed\n" );

        close( sSocket );
        sSocket = 0;

        return -1;
    }

    TCPClientHandle( sSocket,
                     aMessage,
                     aMessageLength );

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

        if ( TCPClient( IPADDRESS,
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
