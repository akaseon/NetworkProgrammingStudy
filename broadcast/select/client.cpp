#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <pthread.h>
#include <string.h>

#define IPADDRESS           (char*)("127.0.0.1")
#define PORT_NO             (5432)

#define MAX_BUFFER_SIZE     (1024)

int gLoop = 0;

void * handleReadEvent( void * aData )
{
    int             sSocket = 0;
    int             sMaxFd = 0;
    fd_set          sReadSet;
    fd_set          sAllSet;
    struct timeval  sTv;
    char            sBuffer[MAX_BUFFER_SIZE];
    int             sBufferLength = 0;

    FD_ZERO( &sReadSet );

    sSocket = *((int *)aData );

    FD_SET( sSocket,
            &sAllSet );
    sMaxFd = sSocket;

    sTv.tv_sec = 5;
    sTv.tv_usec = 0;

    while ( gLoop == 0 )
    {
        sReadSet = sAllSet;

        if ( select( sMaxFd + 1,
                     &sReadSet,
                     NULL,
                     NULL,
                     &sTv )
              > 0 )
        {
            sBufferLength = read( sSocket,
                                  sBuffer,
                                  MAX_BUFFER_SIZE );
            if ( sBufferLength > 0 )
            {
                sBuffer[sBufferLength] = '\0';
                printf("Recv Message : %s\n", sBuffer );
            }
            else
            {
                printf( "read function is failed\n" );
                gLoop = -1;
            }
        }
    }

    return NULL;
}

int connectServer( char     * aIPAddress,
                   short      aPort )
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

    return sSocket;
}

int main( void )
{
    int         sSocket = 0;
    char        sBuffer[MAX_BUFFER_SIZE];
    int         sBufferLength = 0;
    pthread_t   sThread;

    sSocket = connectServer( IPADDRESS,
                             PORT_NO );

    if ( sSocket <= 0 )
    {
        printf( "fail to connectServer\n" );
        return -1;
    }

    gLoop = 0;

    if ( pthread_create( &sThread,
                         NULL,
                         handleReadEvent,
                         (void*)&sSocket )
         != 0 )
    {
        printf( "pthread_create is failed\n" );
        close( sSocket );

        return -1;
    }

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
            if ( write( sSocket, sBuffer, sBufferLength+1 ) < 0 )
            {
                printf( "write Function is failed\n" );
            }
            else
            {
                /* do nothing */
            }
        }
    }

    gLoop = -1;

    pthread_join( sThread, NULL );

    close( sSocket );

    return 0;
}
