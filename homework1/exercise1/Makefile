CC = gcc
CFLAGS = -Wall -g

all: exercise1

exercise1: exercise1.o
	$(CC) $(CFLAGS) -o exercise1 exercise1.o

exercise1.o: exercise1.c
	$(CC) $(CFLAGS) -c exercise1.c

clean:
	rm -f *.o exercise1