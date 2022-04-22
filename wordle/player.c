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
#include "cJSON.h"

/*

*/
void message_player(char *message, Player p)
{
    int numread;
    int count = 0, it = 0;

    while (message[it++] != '\0') {
        count++;
    }

    if ((numread = send(p.socket, message, count, 0)) == -1) {
            printf("server: error sending data packet. \n");
            exit(1);
    }
}

void send_JoinResult(char *res, Player p){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "JoinResult");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "Name", p.name);
    cJSON_AddStringToObject(message, "Result", res);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(package), p);
    cJSON_Delete(message);
}

void send_Chat(char *text, char *sender, Player p){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "Chat");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "Name", sender);
    cJSON_AddStringToObject(message, "Text", text);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(message), p);
    cJSON_Delete(message);
}

void send_StartInstance(Player p, char *server, char *port){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "StartInstance");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "Server", server);
    cJSON_AddStringToObject(message, "Port", port);
    cJSON_AddNumberToObject(message, "Nonce", p.nonce);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(message), p);
    cJSON_Delete(message);
}

void send_JoinInstanceResult(char *res, Player p){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "JoinInstanceResult");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "Name", p.name);
    cJSON_AddNumberToObject(message, "Number", p.num);
    cJSON_AddStringToObject(message, "Result", res);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(message), p);
    cJSON_Delete(message);
}
/* FINISH, BITCH */
void send_StartGame(int rounds, Player *players, Player p){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "StartGame");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddNumberToObject(message, "Rounds", rounds);
    // add_player_array(message, players);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(message), p);
    cJSON_Delete(message);
}

void send_StartRound(int wordlen, int Round, int remain, Player *players, Player p){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "StartRound");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddNumberToObject(message, "WordLength", wordlen);
    cJSON_AddNumberToObject(message, "Round", Round);
    cJSON_AddNumberToObject(message, "RountsRemaining", remain);
    // add_player_array(message, players);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(message), p);
    cJSON_Delete(message);
}

void send_PromptForGuess(int wordlen, Player p, int guessnum){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "PromptForGuess");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "Name", p.name);
    cJSON_AddNumberToObject(message, "GuessNumber", guessnum);
    cJSON_AddNumberToObject(message, "WordLength", wordlen);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(message), p);
    cJSON_Delete(message);
}

void send_GuestResponse(Player p, char *guess, char *ac){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "GuestResponse");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "Name", p.name);
    cJSON_AddStringToObject(message, "Accepted", ac);
    cJSON_AddStringToObject(message, "Guess", guess);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(message), p);
    cJSON_Delete(message);
}

void send_GuessResult(Player p, char *cor, char *time, char *res, char *win){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "GuessResult");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "Name", p.name);
    cJSON_AddStringToObject(message, "Winner", win);
    cJSON_AddStringToObject(message, "Correct", cor);
    cJSON_AddStringToObject(message, "RecriptTime", time);
    cJSON_AddStringToObject(message, "Result", res);
    cJSON_AddNumberToObject(message, "Player", p.num);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(message), p);
    cJSON_Delete(message);
}

void send_EndRound(Player p, char *win){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "EndRound");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "Name", p.name);
    cJSON_AddNumberToObject(message, "Player", p.num);
    cJSON_AddNumberToObject(message, "ScoreEarned", p.score);
    cJSON_AddStringToObject(message, "Winner", win);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(message), p);
    cJSON_Delete(message);
}

/*
    DO THE THING
*/
void send_EndGame(Player p, char *winner, Player *players){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "EndGame");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "WinnerName", winner);
    // add_player_array(message, players);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(message), p);
    cJSON_Delete(message);
}

cJSON *add_player_array(cJSON *json, Player *ps){
    cJSON *package = cJSON_CreateObject();

    return package;
}
