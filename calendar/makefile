CC=gcc
CFLAGS=-I.
DEPS = calendar.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: server.o calendar.o
	$(CC) -o server calendar.o server.o

test: test.o calendar.o
	$(CC) -o test calendar.o test.o

all: server test

clean:
	echo "CLEAN REMOVES DATA TOO"
	rm *.o server test data/*
