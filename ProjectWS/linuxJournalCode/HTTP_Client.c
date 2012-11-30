#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

int create_tcp_socket();
char *get_ip(char *host);
char *build_get_query(char *host, char *page);
char *generate_close_message();
void usage();

#define HOST "localhost"
#define PAGE "/"
#define PORT 3000
#define USERAGENT "HTMLGET 1.0"

int sentImageCount;
int maxImageCount;
struct timespec lastSendTime;
int sock;
char *host;
char *page;
char times[100];

void send_request_on_time(void * vp) {
	while (sentImageCount < maxImageCount) {

		struct timespec currentTime;
		clock_gettime(CLOCK_MONOTONIC, &currentTime);
		uint64_t secondsElapsed = currentTime.tv_sec - lastSendTime.tv_sec;

		// Only send if timer says so.
		if (secondsElapsed > times[sentImageCount]) {
			clock_gettime(CLOCK_MONOTONIC, &lastSendTime);
			sentImageCount++;

			// Build the query
			char *get = build_get_query(host, page);
			fprintf(stderr, "Send Query:\n<<START>>\n%s<<END>>\n", get);

			//Send the query to the server
			int sent = 0;
			int tmpres;
			while (sent < strlen(get)) {
				tmpres = send(sock, get + sent, strlen(get) - sent, 0);
				if (tmpres == -1) {
					perror("Can't send query");
					exit(1);
				}
				sent += tmpres;
			}

			free(get);
		}
	}

	// SEND CLOSE ON end
	char *close = generate_close_message();
	fprintf(stderr, "Send Query:\n<<START>>\n%s<<END>>\n", close);

	//Send the query to the server
	int sent = 0;
	int tmpres;
	while (sent < strlen(close)) {
		tmpres = send(sock, close + sent, strlen(close) - sent, 0);
		if (tmpres == -1) {
			perror("Can't send close query");
			exit(1);
		}
		sent += tmpres;
	}


}

int main(int argc, char **argv) {
	struct sockaddr_in *remote;
	int tmpres;
	char *ip;
	char buf[BUFSIZ + 1];

	int port = PORT;

	if (argc > 1) {
		// Read the port number
		port = atoi(argv[1]);

		// read the times
		if (argc > 2) {
			int i = 2;

			while (i < argc) {
				times[i - 2] = atoi(argv[i]);
				fprintf(stderr, "times[i]   %i \n", times[i]);
				i++;
				maxImageCount++;
			}
		}
	}

	host = HOST;
	page = PAGE;

	sock = create_tcp_socket();
	ip = get_ip(host);

	remote = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in *));
	remote->sin_family = AF_INET;
	tmpres = inet_pton(AF_INET, ip, (void *) (&(remote->sin_addr.s_addr)));
	if (tmpres < 0) {
		perror("Can't set remote->sin_addr.s_addr");
		exit(1);
	} else if (tmpres == 0) {
		fprintf(stderr, "%s is not a valid IP address\n", ip);
		exit(1);
	}
	remote->sin_port = htons(port);

	// CONNECT
	if (connect(sock, (struct sockaddr *) remote, sizeof(struct sockaddr))
			< 0) {
		perror("Could not connect");
		sleep(2);
		exit(1);
	}

	// SEND stuff

	// clock
	sentImageCount = 0;
	clock_gettime(CLOCK_MONOTONIC, &lastSendTime);

	// Setup Sender thread
	pthread_t tid;
	int flag = pthread_create(&tid, /* id */
	NULL, /* attributes */
	send_request_on_time, NULL ); /* routine */

	if (flag < 0) {
		perror("Couldn't create thread");
	}

	// Recieve message code
	memset(buf, 0, sizeof(buf));
	int htmlstart = 0;
	char * messageContent;
	fprintf(stderr, "Start recv \n");
	while ((tmpres = recv(sock, buf, BUFSIZ, 0)) > 0) {
		if (htmlstart == 0) {
			/* Under certain conditions this will not work.
			 * If the \r\n\r\n part is splitted into two messages
			 * it will fail to detect the beginning of HTML content
			 */
			fprintf(stderr, "Receive a page %s  \n", buf);
			messageContent = strstr(buf, "\r\n\r\n");
			if (messageContent != NULL ) {
				htmlstart = 1;
				messageContent += 4;
			}
		} else {
			messageContent = buf;
		}
		if (htmlstart) {
//			fprintf(stdout, messageContent);
		}

		memset(buf, 0, tmpres);
	}
	fprintf(stderr, "HTML content %s \n", messageContent);
	if (tmpres < 0) {
		perror("Error receiving data");
	}
	free(remote);
	free(ip);
	close(sock);

	sleep(1);
	return 0;
}

void usage() {
	fprintf(stderr,
			"USAGE: htmlget host [page]\n\
\thost: the website hostname. ex: coding.debuntu.org\n\
\tpage: the page to retrieve. ex: index.html, default: /\n");
}

int create_tcp_socket() {
	int sock;
	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("Can't create TCP socket");

		sleep(5);

		exit(1);
	}
	return sock;
}

char *get_ip(char *host) {
	struct hostent *hent;
	int iplen = 15; //XXX.XXX.XXX.XXX
	char *ip = (char *) malloc(iplen + 1);
	memset(ip, 0, iplen + 1);
	if ((hent = gethostbyname(host)) == NULL ) {
		herror("Can't get IP");
		exit(1);
	}
	if (inet_ntop(AF_INET, (void *) hent->h_addr_list[0], ip, iplen) == NULL ) {
		perror("Can't resolve host");
		exit(1);
	}
	return ip;
}

char *build_get_query(char *host, char *page) {
	char *query;
	char *getpage = page;
	char *tpl = "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
	if (getpage[0] == '/') {
		getpage = getpage + 1;
		fprintf(stderr, "Removing leading \"/\", converting %s to %s\n", page,
				getpage);
	}
	// -5 is to consider the %s %s %s in tpl and the ending \0
	query = (char *) malloc(
			strlen(host) + strlen(getpage) + strlen(USERAGENT) + strlen(tpl)
					- 5);
	sprintf(query, tpl, getpage, host, USERAGENT);
	return query;
}

char *generate_close_message() {
	char *query = "HTTP/1.1 200 OK\nContent-Type: text/*\nAccept-Ranges: bytes\nConnection: close\n\n";
	return query;
}

