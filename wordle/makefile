CC=gcc -std=gnu99
CFLAGS=-I. -lm 
DEPS = wordle.h cJSON.h player.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: server/server.o wordle.o cJSON.o player.o
	$(CC) -o server/server wordle.o cJSON.o server/server.o player.o -lm

all: server

clean:
	rm *.o server/server server/server.o