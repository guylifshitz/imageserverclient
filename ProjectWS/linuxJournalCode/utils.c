


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "utils.h"

void error_msg(const char* msg, bool halt_flag) {
    perror(msg);
    if (halt_flag) exit(-1); 
}

/* listening socket */
int create_server_socket(bool non_blocking) {
  /* Modify as needed. */
  const int port = 3000;  
  
  struct sockaddr_in server_addr;
  
  /* create, bind, listen */
  int sock = socket(AF_INET,     /* family */
		    SOCK_STREAM, /* TCP */
		    0);          
  if (socket < 0) error_msg("Problem with socket call", true);

  /* non-blocking? */
  if (non_blocking) fcntl(sock, F_SETFL, O_NONBLOCK);
  
  /* bind */
  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port); /* host to network endian */
  if (bind(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) 
    error_msg("Problem with bind call", true);

  /* listen */  
  fprintf(stderr, "Listening for requests on port %i...\n", port);
  if (listen(sock, BACKLOG) < 0)
    error_msg("Problem with listen call", true);

  return sock;
}

void announce_client(struct in_addr* addr) {
  char buffer[BUFF_SIZE + 1];

  inet_ntop(AF_INET, addr, buffer, sizeof(buffer));
  fprintf(stderr, "Client connected from %s...\n", buffer);
}

void generate_echo_response(char request[ ], char response[ ]) {
  strcpy(response, "HTTP/1.1 200 OK\n");        
  strcat(response, "Content-Type: text/*\n");
  strcat(response, "Accept-Ranges: bytes\n"); 
  strcat(response, "Connection: close\n\n");
  strcat(response, request);
}
