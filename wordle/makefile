CC=gcc
CFLAGS=-I.
DEPS = wordle.h cJSON.h player.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: server/server.o wordle.o cJSON.o player.o
	$(CC) -o server/server wordle.o cJSON.o server/server.o player.o

test: test.o wordle.o cJSON.o player.o
	$(CC) -o test wordle.o cJSON.o test.o player.o

all: server test

clean:
	rm *.o server/server server/server.o test 
