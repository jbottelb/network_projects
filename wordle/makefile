CC=gcc
CFLAGS=-I.
DEPS = wordle.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: server.o wordle.o
	$(CC) -o server wordle.o server.o

test: test.o wordle.o
	$(CC) -o test wordle.o test.o

all: server test

clean:
	rm *.o server test data/*
