#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#include "utils.h"
#define PORT 3000
#define SEND_BUFFER_SIZE 10

char *image_1;
int image_1_size;

struct client_data {
	int new_image_request;	// a int which tells us if we have a new request.
	int new_image_pointer;	// pointer to the image's buffer
	int close_requested;	// did we get a close connection request.
	int client;
};

void* handle_client_requests(void *arguments) {
	struct client_data *client_args = (struct client_data *) arguments;

	while (client_args->close_requested == 0) {
		char buffer[BUFF_SIZE + 1 + 10000];
		bzero(buffer, sizeof(buffer));

		fprintf(stderr, "Received a new client request: %s \n", buffer);

		int bytes_read = recv(client_args->client, buffer, sizeof(buffer), 0);
		if (bytes_read < 0)
			error_msg("Problem with recv call", false);

		const char* from = buffer;
		char *to = (char*) malloc(17);
		strncpy(to, from + 58, 17);
		int closeResult = strncmp(to, "Connection: close", 17);
		fprintf(stderr, "Close?: %i \n", closeResult);

		if (closeResult != 0) {
			client_args->new_image_pointer = 0;
			client_args->new_image_request = 1;
		} else {
			client_args->close_requested = 1;
		}
	}
}

/* thread routine */
void* handle_client(void* client_ptr) {
	pthread_detach(pthread_self()); /* terminates on return */

	char *image_to_send;

	/* read/write socket */
	int client = *((int*) client_ptr);

	// create a Client ID (Probably needed for part 2)
	srand(time(NULL ) + client);
	int clientID = rand();

	// SETUP LISTENER FOR NEW REQUESTS

	/* client handler */
	// initalize the client's data structure so we can share information between two threads
	struct client_data client_args;
	client_args.close_requested = 0;
	client_args.new_image_request = 0;
	client_args.new_image_pointer = 0;
	client_args.client = client;

	pthread_t tid;
	int flag = pthread_create(&tid, /* id */
	NULL, /* attributes */
	handle_client_requests, /* routine */
	(void *) &client_args); /* routine's arg */
	if (flag < 0)
		error_msg("Problem creating thread", false);

	// Keep sending stuff until you are told to close.

	fprintf(stderr, "send response \n");

	/* response */
	char response[BUFF_SIZE * 2];
	bzero(response, sizeof(response));
//  generate_echo_response(buffer, response);

	fprintf(stderr, "start send to client# %d \n", clientID);

	int i = 0;

	int image_buffer_send_offset = image_1_size + 1;

	// Loop until the client requested a close (in the pthread)
	while (client_args.close_requested == 0 || image_buffer_send_offset < image_1_size) {

		// If the client sent a new request, we should reset everything
		if (client_args.new_image_request == 1) {
			client_args.new_image_request = 0;
			image_buffer_send_offset = 0;// restart the offset to 0, so we read from the start.
		}

		// If this is the first time we are sending bytes from this image send a message to the client indicating this fact
		if (image_buffer_send_offset == 0) {
			int bytes_written = write(client, "NEW IMAGE", sizeof("NEW IMAGE"));
		}

		// If we havn't sent the whole image, keep sending
		if (image_buffer_send_offset < image_1_size) {

			// Check the buffer isnt going to overflow
			int bufferSizeToSend = SEND_BUFFER_SIZE;
			if (image_buffer_send_offset + SEND_BUFFER_SIZE > image_1_size) {
				bufferSizeToSend = image_1_size - image_buffer_send_offset;
			}

			// write to the buffer (from the right offset, and the right amount of data)
			int bytes_written = write(client,
					&image_1[image_buffer_send_offset], SEND_BUFFER_SIZE);

			// need to sleep some amount of time or write acts oddly and sends all the string in one go eventually.
			usleep(1);		// sleep a microsecond or something small like that
		}

		image_buffer_send_offset += SEND_BUFFER_SIZE;
	}

	fprintf(stderr, "end send on client: %d \n", clientID);

	// TODO: IF SHOULD CLOSE, THEN EXIT LOOP AND CLOSE
	close(client);

	return NULL ;
} /* detached thread terminates on return */

int load_images() {

	//	image_1_size = ae_load_file_to_memory("images/big-test.txt", &image_1);
//	image_1_size = ae_load_file_to_memory("images/test2.txt", &image_1);
//		image_1_size = ae_load_file_to_memory("images/test.txt", &image_1);
		image_1_size = ae_load_file_to_memory("images/1.jpg", &image_1);

//		fprintf(stderr, "LOAD image_1: sizeof: %d   size %d ,   string: %s \n", sizeof (image_1), size, image_1);

	if (image_1_size < 0) {
		puts("Error loading file image_1");
		return 1;
	}

	return 0;
}

int main(int argc, char **argv) {

	int port = PORT;

	if (argc == 2) {
		port = atoi(argv[1]);
	}

	char buffer[BUFF_SIZE + 1];

	// load images
	load_images();

	struct sockaddr_in client_addr;
	socklen_t len = sizeof(struct sockaddr_in);

	/* listening socket */
	int sock = create_server_socket(false, port);

	/* connections */
	while (true) {
		int client = accept(sock, (struct sockaddr*) &client_addr, &len);
		if (client < 0)
			error_msg("Problem accepting a client request", true);

		announce_client(&client_addr.sin_addr);

		/* client handler */
		pthread_t tid;
		int flag = pthread_create(&tid, /* id */
		NULL, /* attributes */
		handle_client, /* routine */
		&client); /* routine's arg */
		if (flag < 0)
			error_msg("Problem creating thread", false);
	}

	return 0;
}
