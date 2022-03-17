FLAGS= -Wall

all: server client

client:
	echo "Haha python doesn't need to be compiled"

server: server.o
	gcc server.c -o run_server

%.o: %.c
	gcc ${FLAGS} -c $< -o $@

clean:
	rm *.o
