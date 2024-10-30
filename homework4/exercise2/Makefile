# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Targets
all: server client

# Server compilation
server: server.c
	$(CC) $(CFLAGS) -o server server.c

# Client compilation
client: client.c
	$(CC) $(CFLAGS) -o client client.c

# Clean up generated files
clean:
	rm -f server client
