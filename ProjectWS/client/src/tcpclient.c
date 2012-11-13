#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "connect_client.h"

const char *DAYTIME_PORT="13111";


int
main(int argc, char *argv[])
{
    int connfd;
    char *myhost;
    char timeStr[256];

    myhost = "localhost";
    if (argc > 1)
        myhost = argv[1];

    connfd = connect_client(myhost, DAYTIME_PORT, AF_UNSPEC, SOCK_STREAM);

    if (connfd < 0) {
         fprintf(stderr,
                 "client error:: could not create connected socket "
                 "socket\n");
         return -1;
    }

    memset(timeStr, 0, sizeof(timeStr));

    while (read(connfd, timeStr, sizeof(timeStr)) > 0) 
        printf("Out: %s", timeStr);

    close(connfd);

    return 0;
}





