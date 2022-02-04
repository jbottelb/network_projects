FLAGS= -Wall

all: server client

server: server/server.o
	gcc server/server.o -o run_server
	gcc server/server.o -o server/run_server

client: client/client.o
	gcc client/client.c -o run_client
	gcc client/client.c -o client/run_client

%.o: %.c
	gcc ${FLAGS} -c $< -o $@

clean:
	rm run_client run_server server/run_server client/run_client server/*.o client/*.o 