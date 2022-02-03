
all: server client

server: server/server.c
	gcc -o run_server server/server.c

client: client/client.c
	gcc -o run_client client/client.c

clean:
	rm run_client run_server
