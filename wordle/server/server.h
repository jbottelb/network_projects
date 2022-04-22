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
void send_result_to_client(int sock_fd, int success, request *req);
void send_result_to_client_with_data(int sock_fd, int success, request *req, event **events);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa);
void start_game(char *port);
