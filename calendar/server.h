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
#include "calendar.h"

void sigchld_handler(int s);
request *accept_request(int sock);
void send_file_to_socket(FILE* file_fd, int sock_fd);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa);
