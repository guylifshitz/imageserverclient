


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <signal.h>
#include "utils.h"

int main() {
  /* Avoid zombies. */
  signal(SIGCHLD, SIG_IGN);

  char buffer[BUFF_SIZE + 1];      

  struct sockaddr_in client_addr;
  socklen_t len = sizeof(struct sockaddr_in);

  /* listening socket */
  int sock = create_server_socket(false);

  /* connections + requests */
  while (true) {
     int client = accept(sock, 
			(struct sockaddr*) &client_addr, 
			&len);
    if (client < 0) error_msg("Problem with accept call", true);

    announce_client(&client_addr.sin_addr);

    /* fork child */
    pid_t pid = fork();
    if (pid < 0) error_msg("Problem with fork call", false);

    /* 0 to child, child's PID to parent */
    if (0 == pid) {  /** child **/
      close(sock);   /* child's listening socket */

      /* request */
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
      exit(0);       /* terminate */
    } 
    else             /** parent **/
      close(client); /* parent's read/write socket. */
  } 

  return 0; 
}
