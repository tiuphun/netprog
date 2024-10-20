CC = gcc
CFLAGS = -Wall -g

all: server client

server: server.o
	$(CC) $(CFLAGS) -o server server.o

client: client.o
	$(CC) $(CFLAGS) -o cl