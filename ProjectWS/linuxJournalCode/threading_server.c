



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#include "utils.h"

char *image_1;
int image_1_size;

/* thread routine */
void* handle_client(void* client_ptr) {
  pthread_detach(pthread_self()); /* terminates on return */


  /* read/write socket */
  int client = *((int*) client_ptr);


  srand (time(NULL)+client);
  int clientID = rand();

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

//  generate_image_response(image_1, response);
//  sleep (rand()/100000000);
  //  int bytes_written = send(client, response, strlen(response), 0);
  fprintf(stderr,"start send %d \n", clientID);
//  int bytes_written = send(client, image_1, 20000, 0);
  int i = 0;

  do{
	  //	  int bytes_written = write(client, image_1[image_1_size/10 * i], image_1_size/10 );
	  int bytes_written = write(client, &image_1[image_1_size-100], 100 );
	  if (bytes_written < 0) error_msg("Problem with send call", false);
  }
  while(i < 10);

  fprintf(stderr,"end  send %d \n", clientID);

  close(client); 

  return NULL;
} /* detached thread terminates on return */

int load_images()
{

	//	size = ae_load_file_to_memory("images/test.txt", &image_1);
		image_1_size = ae_load_file_to_memory("images/big-test.txt", &image_1);
//		size = ae_load_file_to_memory("images/1.jpg", &image_1);

//		fprintf(stderr, "LOAD image_1: sizeof: %d   size %d ,   string: %s \n", sizeof (image_1), size, image_1);

	if (image_1_size  < 0)
	{
		puts("Error loading file image_1");
		return 1;
	}


	fprintf(stderr, "IMAGE PART : %s \n", &image_1[image_1_size-100]);



	return 0;
}

int main() {  
  char buffer[BUFF_SIZE + 1];      
  
  // load images
  load_images();

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
