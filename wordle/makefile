CC=gcc
CFLAGS=-I.
DEPS = wordle.h cJSON.h player.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: server/server.o wordle.o cJSON.o
	$(CC) -o server/server wordle.o cJSON.o server/server.o

test: test.o wordle.o cJSON.o
	$(CC) -o test wordle.o cJSON.o test.o

all: server test

clean:
	rm *.o server/server server/server.o test
