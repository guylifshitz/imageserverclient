



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#include "utils.h"

/* thread routine */
void* handle_client(void* client_ptr) {
  pthread_detach(pthread_self()); /* terminates on return */

  /* read/write socket */
  int client = *((int*) client_ptr);

  /* request */
  char buffer[BUFF_SIZE + 1];
  bzero(buffer, sizeof(buffer));
  int bytes_read = recv(client, buffer, sizeof(buffer), 0); 
  if (bytes_read < 0) error_msg("Problem with recv call", false);

  /* response */
  char response[BUFF_SIZE * 2]; 
  bzero(response, sizeof(response));
  generate_echo_response(buffer, response);
  int bytes_written = send(client, response, strlen(response), 0); 
  if (bytes_written < 0) error_msg("Problem with send call", false);

  close(client); 

  return NULL;
} /* detached thread terminates on return */

int main() {  
  char buffer[BUFF_SIZE + 1];      
  
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
