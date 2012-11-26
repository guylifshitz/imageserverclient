all: servers

servers: iterative_server forking_server threading_server polling_server 

iterative_server: iterative_server.c utils.c utils.h
	gcc -o iterative_server iterative_server.c utils.c -I.
forking_server: forking_server.c utils.c utils.h
	gcc -o forking_server forking_server.c utils.c -I.
threading_server: threading_server.c utils.c utils.h
	gcc -o threading_server threading_server.c utils.c -lpthread -I.
polling_server: polling_server.c utils.c utils.h
	gcc -o polling_server polling_server.c utils.c -I.


