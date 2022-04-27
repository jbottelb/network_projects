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
#include <sys/time.h>
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
Player *create_player(char *name, int socket, int num, int nonce);

void message_player(char *message, Player *p);
void send_JoinResult(char *res, Player *p);
void send_Chat(char *text, char *sender, Player *p);
void send_StartInstance(Player *p, char *server, char *port);
void send_JoinInstanceResult(char *res, Player *p);
void send_StartGame(int rounds, Player **players, Player *p, int num_players);
void send_StartRound(int wordlen, int Round, int remain, Player **players, Player *p, int num_players);
void send_PromptForGuess(int wordlen, Player *p, int guessnum);
void send_GuessResponse(Player *p, char *guess, char *ac);
void send_GuessResult(Player *p, Player **ps, char *win, int num_players);
void send_EndRound(Player *p, Player **ps, int rem, int num_players);
void send_EndGame(Player *p, char *winner, Player **players, int num_players);

// each one will take the full JSON obj
Player *recv_Join(cJSON *message, int sock, int num, int nonce);
char   *recv_Chat(cJSON *message);  // returns string PlayerName: text
Player *recv_JoinInstance(cJSON *message, int sock, int num);
char   *recv_Guess(cJSON *message, Player *p);

cJSON *add_player_array(cJSON *json, Player **ps, int num_players);
cJSON *add_player_array_gr(cJSON *json, Player **ps, int num_players);
cJSON *add_player_array_er(cJSON *json, Player **ps, int num_players);


Player *find_player(Player **ps, char *name, int players);
char *find_winner(Player **ps, int players);
#endif
