CC = gcc
CFLAGS = -Wall -g

all: exercise1 exercise2

exercise1: exercise1.o
	$(CC) $(CFLAGS) -o exercise1 exercise1.o

exercise1.o: exercise1.c
	$(CC) $(CFLAGS) -c exercise1.c

exercise2: exercise2.o
	$(CC) $(CFLAGS) -o exercise2 exercise2.o

exercise2.o: exercise2.c
	$(CC) $(CFLAGS) -c exercise2.c

clean:
	rm -f *.o exercise1 exercise2