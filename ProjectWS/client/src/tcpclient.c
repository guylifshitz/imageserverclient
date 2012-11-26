#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "connect_client.h"

const char *DAYTIME_PORT = "3000";
#define NTHREADS 20


void * create_client(){

	srand(time(NULL));

	int i = rand();
	printf("Try to connect %d \n", i);

	int connfd;
	char *myhost;
	char timeStr[256];

	myhost = "localhost";

	connfd = connect_client(myhost, DAYTIME_PORT, AF_UNSPEC, SOCK_STREAM);

	if (connfd < 0) {
		fprintf(stderr, "client error:: could not create connected socket "
				"socket\n");
		return -1;
	}

	memset(timeStr, 0, sizeof(timeStr));

	while (read(connfd, timeStr, sizeof(timeStr)) > 0)
		printf("Client Out:%d \n %s \n", i, timeStr);

	close(connfd);

}

int main(int argc, char *argv[]) {
	int connfd;
	char *myhost;
	char timeStr[256];

	pthread_t thread_id[NTHREADS];
	int i;

	for (i = 0; i < NTHREADS; i++) {
		printf("For loop\n");
		int retVal = pthread_create(&thread_id[i], NULL, create_client, NULL );

		if (retVal){
			printf("Thread failed\n");
		}
		else
		{
			printf("Thread worked\n");
		}
	}
	for (i = 0; i < NTHREADS; i++) {
		pthread_join(thread_id[i], NULL);
	}

/*
	int iter = 0;
	while (iter < 100) {
		iter++;
		myhost = "localhost";
		if (argc > 1)
			myhost = argv[1];

//    DAYTIME_PORT = DAYTIME_PORT;
		connfd = connect_client(myhost, DAYTIME_PORT, AF_UNSPEC, SOCK_STREAM);

		if (connfd < 0) {
			fprintf(stderr, "client error:: could not create connected socket "
					"socket\n");
			return -1;
		}

		memset(timeStr, 0, sizeof(timeStr));

		while (read(connfd, timeStr, sizeof(timeStr)) > 0)
			printf("Out: %s \n", timeStr);

		close(connfd);
	}*/
	return 0;
}

