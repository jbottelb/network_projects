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
#include "cJSON.h"

typedef enum {
    GUESSED, GUESSING, WAITING
} PlayerState;

typedef struct Player Player;
struct Player {
    int socket;
    int score;
    int nonce;
    int num;
    char *name;
    char *res;
    char *rec_time;
    char *correct;      // yes or no
    char *winner;       // yes or no
    PlayerState state;
};

// TODO: Create player function

void message_player(char *message, Player *p);
void send_JoinResult(char *res, Player *p);
void send_Chat(char *text, char *sender, Player *p);
void send_StartInstance(Player *p, char *server, char *port);
void send_JoinInstanceResult(char *res, Player *p);
void send_StartGame(int rounds, Player **players, Player *p);
void send_StartRound(int wordlen, int Round, int remain, Player **players, Player *p);
void send_PromptForGuess(int wordlen, Player *p, int guessnum);
void send_GuessResponse(Player *p, char *guess, char *ac);
void send_GuessResult(Player *p, Player **ps, char *win);
void send_EndRound(Player *p, Player **ps, int rem);
void send_EndGame(Player *p, char *winner, Player **players);

cJSON *add_player_array(cJSON *json, Player **ps);
cJSON *add_player_array_gr(cJSON *json, Player **ps);
cJSON *add_player_array_er(cJSON *json, Player **ps);

#endif
