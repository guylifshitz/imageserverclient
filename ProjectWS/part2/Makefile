all: clients

clients: HTTP_Client 

HTTP_Client: HTTP_Client.c 
	gcc -o HTTP_Client HTTP_Client.c -lpthread -lrt -I.

all: servers

servers: threading_server 

threading_server: threading_server.c utils.c utils.h
	gcc -o threading_server threading_server.c utils.c `libslack-config --cflags --libs` -lpthread -I.


