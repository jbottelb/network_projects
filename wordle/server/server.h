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


void sigchld_handler(int s);
char *accept_request(int sock);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa);
void start_game(char *new_port, Player **players, int nonce, int set_players, int set_rounds, char* dict_file);
void handle_guess(Player *p, cJSON *data, Wordle *w, char *dict_file);
