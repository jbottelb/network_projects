#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// ./proj1server 10584
// TODO: get port from CL
#define PORT "41001"

int create_and_bind_socket(char*);
char* get_file_name(int sock);

int main(){
    int sock = create_and_bind_socket(PORT);
    
    while (1)
    {
        // listen for connection
        listen(sock, 10);
        if (sock) { // if we have a connection
            // read request and send file
            char* file_name = get_file_request(sock);
            // search for file, get size

            // sendsize of file to client

            // send file to client
            
        }
    }

    return 0;
}

char* get_file_name(int sock){
    // get the name of the file to send to the client
    return "";
}

int create_and_bind_socket(char* port){
    // used Beej's guide
    // create
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int sockfd, new_fd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    getaddrinfo(NULL, port, &hints, &res);
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    bind(sockfd, res->ai_addr, res->ai_addrlen);

    return sockfd;
}