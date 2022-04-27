CC=gcc 
CFLAGS=-I. -lm 
DEPS = wordle.h cJSON.h player.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: server/server.o wordle.o cJSON.o player.o
	$(CC) -o server/server wordle.o cJSON.o server/server.o player.o -lm

test: test.o wordle.o cJSON.o player.o
	$(CC) -o test wordle.o cJSON.o test.o player.o -lm

all: server test

clean:
	rm *.o server/server server/server.o test
