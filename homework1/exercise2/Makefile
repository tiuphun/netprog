CC = gcc
CFLAGS = -Wall

all: main

main: main.o scoreboard.o helpers.o
	$(CC) $(CFLAGS) -o main main.o scoreboard.o helpers.o

main.o: main.c scoreboard.h
	$(CC) $(CFLAGS) -c main.c

scoreboard.o: scoreboard.c scoreboard.h
	$(CC) $(CFLAGS) -c scoreboard.c

helpers.o: helpers.c scoreboard.h
	$(CC) $(CFLAGS) -c helpers.c

clean:
	rm -f *.o main
