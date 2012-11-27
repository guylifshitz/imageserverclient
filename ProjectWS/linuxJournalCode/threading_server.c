



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#include "utils.h"

char *image_1;

/* thread routine */
void* handle_client(void* client_ptr) {
  pthread_detach(pthread_self()); /* terminates on return */

  /* read/write socket */
  int client = *((int*) client_ptr);

  /* request */
  char buffer[BUFF_SIZE + 1 + 10000];
  bzero(buffer, sizeof(buffer));
  int bytes_read = recv(client, buffer, sizeof(buffer), 0); 
  if (bytes_read < 0) error_msg("Problem with recv call", false);

  fprintf(stderr,"send response \n");

  /* response */
  char response[BUFF_SIZE * 2]; 
  bzero(response, sizeof(response));
//  generate_echo_response(buffer, response);

  fprintf(stderr, "image_1 3: %s \n", &image_1);
  fprintf(stderr, "LOAD image_1 2: size %d ,   string: %s \n", sizeof (image_1), image_1);

  generate_image_response(image_1, response);

  //  int bytes_written = send(client, response, strlen(response), 0);
  int bytes_written = send(client, image_1, strlen(image_1), 0);
  if (bytes_written < 0) error_msg("Problem with send call", false);
  fprintf(stderr,"sent response \n");

  close(client); 

  return NULL;
} /* detached thread terminates on return */

int load_images()
{
	int size;
	//	size = ae_load_file_to_memory("images/test.txt", &image_1);
		size = ae_load_file_to_memory("images/1.jpg", &image_1);

		fprintf(stderr, "LOAD image_1: sizeof: %d   size %d ,   string: %s \n", sizeof (image_1), size, image_1);

	if (size < 0)
	{
		puts("Error loading file");
		return 1;
	}


	return 0;
}

int main() {  
  char buffer[BUFF_SIZE + 1];      
  
  // load images
  load_images(image_1);

  struct sockaddr_in client_addr;
  socklen_t len = sizeof(struct sockaddr_in);

  /* listening socket */
  int sock = create_server_socket(false);

  /* connections */
  while (true) {
    int client = accept(sock, 
			(struct sockaddr*) &client_addr, 
			&len);
    if (client < 0) error_msg("Problem accepting a client request", true);

    announce_client(&client_addr.sin_addr);

    /* client handler */
    pthread_t tid;
    int flag = pthread_create(&tid,          /* id */
			      NULL,          /* attributes */
			      handle_client, /* routine */
			      &client);      /* routine's arg */
    if (flag < 0) error_msg("Problem creating thread", false);
  } 

  return 0; 
}
