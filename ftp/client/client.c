/*
** client.c -- a stream socket client demo
  we ripped this all from beej but thats what you did too sooo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>

#define SIZE 1024 // max number of bytes we can get at once

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[SIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 4){
        fprintf(stderr, "youre using it wrong \n");
        exit(1);
    }

    char *ip = argv[1];
    char *port = argv[2];
    char *file_name = argv[3];
    printf("%s %s %s\n", ip, port, file_name);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], port, &hints, &servinfo)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next){
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }

    if (p == NULL){
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    if (send(sockfd, file_name, strlen(file_name), 0) == -1)
        perror("send");


    char size[SIZE];
    double file_length;
    int len;
    
    if ((len = ntohl(recv(sockfd, size, sizeof(size), 0))) == -1) {
        printf("receive\n");
        exit(1);
    }

    if (len != 0) {
        if (atoi(size) < 0) {
            printf("File does not exist (client)\n");
            exit(1);
        }
        else {
            file_length = atoi(size);
        }
    }
    bzero(size, SIZE);

    char buffer[SIZE];
    FILE *fp = fopen(file_name, "w");
    int bytes_received = 0;
    struct timeval recv_start, recv_end;

    
    gettimeofday(&recv_start, NULL);

    while (bytes_received < file_length) {
        if ((len = recv(sockfd, buffer, sizeof(buffer), 0)) <= 0) {
            printf("oof moment\n");
            exit(1);
        }

        bytes_received += len;

        fwrite(buffer, sizeof(char), len, fp);
        bzero(buffer, SIZE);
    }

    gettimeofday(&recv_end, NULL);
    close(sockfd);
    fclose(fp);

    double total_time = (1000000*recv_end.tv_sec+recv_end.tv_usec) - (1000000*recv_start.tv_sec+recv_start.tv_usec);

    printf("%lf bytes transferred in %lf seconds for a speed of %lf MB/s.\n", file_length, (total_time / 1000000.), (file_length / 1000000.) / (total_time / 1000000.));

    return 0;
}
