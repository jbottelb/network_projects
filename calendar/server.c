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

Calendar *calendars[128]; 

void handler(int new_fd)
{
    // handles the requests from the client
    // (I am outlining to show how to use the calendar class)
    // (handle the input file like a serialization of requests)
    
    // get request from client
    // build request with string
    // request *req = create_request(revieved string (malloced))
    request *req = accept_request(new_fd);
    Calendar *cal;

    int flag = 1;

    // REMEMBER: JSON MUST BE MALLOCED AND FREE

    // load appropriate calendar
    // Calendar *cal = load_calendar(name + .cal, name)
    for (int i = 0; i < 128; i++) {
        if(calendars[i]) {
            if (strcmp(calendars[i]->name, req->calName) == 0) {
                flag = 0;
                cal = calendars[i];
                break;
            }
        }
    }

    if (flag == 1) {
        char *path = (char *)calloc(BUFSIZ, sizeof(char));
        char *folder = "data/";

        path = strcpy(path, folder);
        path = strcat(path, req->calName);

        cal = load_calendar(path, req->calName);

        for (int i = 0; i < 128; i++) {
            if (!calendars[i]) {
                calendars[i] = cal;
                break;
            }
        }
    }

    int success;
    
    switch(req->type) {
        case 0:
        case 1:
        case 2:
        {
            cal = process_edit_request(req, cal);
            success = save_request(req, cal);
            send_result_to_client(new_fd, success, req);
            break;
        }
        case 3:
        {
            event **events = get_events_by_date(cal, req->param);
            
            if (!events) {success = 1;}
            else {success = 0;}

            send_result_to_client_with_data(new_fd, success, req, events);
            free(events);
            break;
        }
        case 4:
        {
            event **events = get_events_by_range(cal, req->param);

            if (!events) {success = 1;}
            else {success = 0;}

            send_result_to_client_with_data(new_fd, success, req, events);
            free(events);
            break;
        }
        default:
            printf("invalid request type \n");
    }
    /*
    // unleash
    close_request(req);
    free(cal);
    */
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
    int mt = -1;

    port = PORT;
    
    if (argc == 2)
        mt = 0;

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

        if (mt != -1) {
            signal(SIGCHLD, SIG_IGN);

	        /* Fork off child process to handle request */
            pid_t pid = fork();

            if(pid < 0){
                continue;
            }
            else if(pid == 0) {
                handler(new_fd);
                exit(0);
            }
        }
        else {
            handler(new_fd);
        }

        // Closes socket and returns to listening for new connections
        close(new_fd);
    }

    return 0;
}

void send_result_to_client(int sockfd, int success, request *req)
{
    int numread;
    char *data = (char *)calloc(BUFSIZ, sizeof(char));

    strcat(data, "{\"command\": \"");
    if (req->type == 0) {
        strcat(data, "add");
    }
    else if (req->type == 1) {
        strcat(data, "remove");
    }
    else if (req->type == 2) {
        strcat(data, "update");
    }

    strcat(data, "\", \"calendar\": \"");
    strcat(data, req->calName);
    strcat(data, "\", \"identifier\": \"");

    if (success == 0) {
        char *ident = (char *)calloc(BUFSIZ, sizeof(char));
        sprintf(ident, "%d", req->event->identifier);

        strcat(data, ident);
        strcat(data, "\", \"success\": \"");
        strcat(data, "True\", \"error\": \"None\", \"data\": \"None\"}");
    }
    else {
        strcat(data, "XXXX");
        strcat(data, "\", \"success\": \"");
        strcat(data, "False\", \"error\": \"Command failed to execute in the calendar\", \"data\": \"None\"}");
    }

    // Handles edge case where the read is smaller than the buffer size
    if ((numread = send(sockfd, data, sizeof(data), 0)) == -1) {
            printf("server: error sending data packet. \n");
            exit(1);
    }
}

void send_result_to_client_with_data(int sockfd, int success, request *req, event **events)
{
    int numread;
    char *data = (char *)calloc(BUFSIZ, sizeof(char));

    strcat(data, "{\"command\": \"");
    if (req->type == 3) {
        strcat(data, "get");
    }
    else if (req->type == 4) {
        strcat(data, "getall");
    }

    strcat(data, "\", \"calendar\": \"");
    strcat(data, req->calName);
    strcat(data, "\", \"identifier\": \"");

    if (success == 0) {
        strcat(data, "event identifiers in data");
        strcat(data, "\", \"success\": \"");
        strcat(data, "True\", \"error\": \"None\", \"data\": [ ");
        
        int i = 0;
        while(events[i]) {
            if (i != 0) {
                strcat(data, ", ");
            }
            char *e = (char *)calloc(BUFSIZ, sizeof(char));
            e = string_from_event(events[i]);
            strcat(data, e);
            i++;
        }

        strcat(data, " ]");
    }
    else {
        strcat(data, "XXXX");
        strcat(data, "\", \"success\": \"");
        strcat(data, "True\", \"error\": \"None\", \"data\": \"No events in the date range\"}");
    }

    // Handles edge case where the read is smaller than the buffer size
    if ((numread = send(sockfd, data, sizeof(data), 0)) == -1) {
            printf("server: error sending data packet. \n");
            exit(1);
    }
}

request *accept_request(int sock)
{
    int file_size, numbytes;
    char buf[SIZE];

    // Receives file name from client
    if ((numbytes = recv(sock, buf, SIZE-1, 0)) == -1){
        printf("server: request was not received. \n");
        exit(1);
    }

    // Add null terminator
    buf[numbytes] = '\0';
    printf("server: received request\n%s\n", buf);

    request *request = request_from_string(buf);
    return request;
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
