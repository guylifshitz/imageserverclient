#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include "utils.h"

int main() {
  /* Buffer for I/O operations. */
  char buffer[BUFF_SIZE + 1];      

  /* Arguments for the accept call. */
  struct sockaddr_in client_addr;
  socklen_t len = sizeof(struct sockaddr_in);

  /* Create a server socket to accept client connections. */
  int sock = create_server_socket(false);

  /* Accept requests indefinitely. */
  while (true) {
    /* Get the client socket for reading/writing. */
    int client = accept(sock, 
			(struct sockaddr*) &client_addr, 
			&len);
    if (client < 0) error_msg("Problem accepting a client request", true);

    announce_client(&client_addr.sin_addr);

    /* Read a client request. */
    bzero(buffer, sizeof(buffer));
    int bytes_read = recv(client, buffer, sizeof(buffer), 0); 
    if (bytes_read < 0) error_msg("Problem with recv call", false);
    
    /* Send a response. */
    char response[BUFF_SIZE * 2]; /* twice as big to be safe */
    bzero(response, sizeof(response));
    generate_echo_response(buffer, response);
    int bytes_written = send(client, response, strlen(response), 0); 
    if (bytes_written < 0) error_msg("Problem with send call", false);

    close(client); 
  } /* while(true) */

  return 0; 
}
