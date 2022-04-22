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
#include "player.h"
#include "wordle.h"
#include "cJSON.h"

#define PLAYERS 1

void sigchld_handler(int s);
char *accept_request(int sock);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa);
void start_game(char *port);
int in_word_list(char *word);
void handle_guess(Player *p, cJSON *data, Wordle *w);
