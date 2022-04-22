#ifndef PLAYER_H
#define PLAYER_H

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

typedef enum {
    GUESSED, GUESSING, WAITING
} PlayerState;

typedef struct Player Player;
struct Player {
    int socket;
    int score;
    PlayerState state;
};

void message_player(char *message, Player p);


#endif
