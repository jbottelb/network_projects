/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "server.h"
#include "calendar.h"


#define PORT "41069"  // the port users will be connecting to
#define SIZE 1000
#define BACKLOG 10   // how many pending connections queue will hold

int handler()
{
    // handles the requests from the client
    // (I am outlining to show how to use the calendar class)
    // (handle the input file like a serialization of requests)

    // take request from client

    // build request with string
    // REMEMBER: JSON MUST BE MALLOCED AND FREE
    // request *req = create_request(revieved strin (malloced))


    // load appropriate calendar
    // Calendar *cal = load_calendar(name + .cal, name)

    // IF IT ISNT THERE, IT WILL HAVE TO BE CREATED.

    /*
    switch (REQUEST TYPE):
        case ADD_EVENT or REMOVE or UPDATE:
            (this handles all disk stuff)
            cal = process_edit_request(request string, type, cal);
        case GET:
            event **events = get_events_by_date(Calendar *cal, char* date);
            // build return string off of this
        case GET_RANGE:
            event **events = get_events_by_range(cal, start, end)
            // build return string off of this

    if we die, free the calendar firsts
    free request too (close_request)
    */
    return 0;
}

int main(int argc, char *argv[])
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    char *port;

    if (argc == 2)
        port = argv[1];
    else
        port = PORT;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (!p)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);

        // Get file name from client
        char* file_name = get_filename_from_client(new_fd);

        // Check if filename exists and open it
        FILE *fp = fopen(file_name, "r");
        if (!fp)
        {
          printf("server: file does not exist. \n");
          close(new_fd);
          exit(1);
        }

        // Move file pointer to end of file
        if (fseek(fp, 0L, SEEK_END) != 0) {
            printf("server: error in reaching EOF. \n");
            exit(1);
        }

        // Grab size of file from file pointer
        uint32_t file_size = ftell(fp);

        // Convert from host order to network long order
        file_size = htonl(file_size);

        // Send file size to client
        if (send(new_fd, (char *) &file_size, sizeof(uint32_t), 0) == -1) {
            printf("server: file size failed to send. \n");
            exit(1);
        }

        // Move file pointer back to start of file for reading
        if (fseek(fp, 0L, SEEK_SET) != 0) {
            printf("server: error in reaching beginning of file. \n");
            exit(1);
        }

        // Sends the file in packets to client
        send_file_to_socket(fp, new_fd);

        // Closes socket and returns to listening for new connections
        close(new_fd);
    }

    return 0;
}



void send_file_to_socket(FILE* fp, int sockfd)
{
    int numbytes, numread;
    char data[SIZE];

    // Loop until all data is sen
    while((numbytes = fread(data, sizeof(char), SIZE, fp)) == SIZE) {
        // Sends the packet to the client
        if ((numread = send(sockfd, data, sizeof(data), 0)) == -1) {
            printf("server: error sending data packet. \n");
            exit(1);
        }
        bzero(data, SIZE);
    }

    // Handles edge case where the read is smaller than the buffer size
    if ((numread = send(sockfd, data, numbytes, 0)) == -1) {
            printf("server: error sending data packet. \n");
            exit(1);
    }
}

char *get_filename_from_client(int sock)
{
    int file_size, numbytes;
    char buf[SIZE];

    // Receives file name length from client
    if ((file_size = recv(sock, buf, 2, 0)) == -1) {
        printf("server: file name length was not received. \n");
        exit(1);
    }

    // Receives file name from client
    if ((numbytes = recv(sock, buf, SIZE-1, 0)) == -1){
        printf("server: file name was not received. \n");
        exit(1);
    }

    // Add null terminator
    buf[numbytes] = '\0';
    printf("server: received file_name '%s'\n", buf);

    char *buff_pointer = buf;
    return buff_pointer;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}
