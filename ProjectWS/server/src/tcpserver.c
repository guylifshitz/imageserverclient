#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

#include "listen_server.h"

const char *DAYTIME_PORT="13111";

int
main(int argc, char *argv[])
{
    int listenfd, connfd;
    socklen_t addrlen;
    char timeStr[256];
    struct sockaddr_storage clientaddr;
    time_t now;
    char clienthost[NI_MAXHOST];
    char clientservice[NI_MAXSERV];

    /* local server socket listening at daytime port=13 */
    listenfd = listen_server(NULL, DAYTIME_PORT, 
                             AF_UNSPEC, SOCK_STREAM);

    if (listenfd < 0) {
         fprintf(stderr,
                 "listen_socket error:: could not create listening "
                 "socket\n");
         return -1;
    }

    for ( ; ;) {
        addrlen = sizeof(clientaddr);

        /* accept daytime client connections */
        connfd = accept(listenfd, 
                        (struct sockaddr *)&clientaddr, 
                        &addrlen);

        if (connfd < 0) 
            continue;

        memset(clienthost, 0, sizeof(clienthost));
        memset(clientservice, 0, sizeof(clientservice));

        getnameinfo((struct sockaddr *)&clientaddr, addrlen,
                    clienthost, sizeof(clienthost),
                    clientservice, sizeof(clientservice),
                    NI_NUMERICHOST);

        printf("Received request from host=[%s] port=[%s]\n",
               clienthost, clientservice);

        int iter = 0;
        while(iter < 100000)
        {
            /* process daytime request from a client */
            memset(timeStr, 0, sizeof(timeStr));
            time(&now);
            sprintf(timeStr, "%s %d", ctime(&now), iter);

        	iter ++;

        	write(connfd, timeStr, strlen(timeStr));
        }
        close(connfd);
    }

    return 0;
}
