CC=gcc
CFLAGS=-I.
DEPS = calendar.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: server.o calendar.o
	$(CC) -o server calendar.o server.o

clean:
	rm *.o server data/*
