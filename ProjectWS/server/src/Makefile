CODE_DIR = src
CXX=gcc
CXXFLAGS=-pedantic -Wall

LD=$(CXX)
LDFLAGS=$(CXXFLAGS)

all: tcpserver udpserver tcpclient udpclient $(CODE_DIR)

tcpserver: listen_server.o 
udpserver: listen_server.o 
tcpclient: connect_client.o
udpclient: connect_client.o

listen_server.o:
connect_client.o:

clean:
	rm -f *.o tcpserver udpserver tcpclient udpclient

