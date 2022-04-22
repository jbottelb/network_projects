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
#include "server/server.h"

#define SIZE 1024

Player *create_player(char *name, int socket, int num, int nonce){
    Player *new = (Player *)calloc(1, sizeof(Player));
    new->name = name;
    new->socket = socket;
    new->num = num;
    new->nonce = nonce;
    new->winner = "no";
    new->state = WAITING;
    new->correct = "no";

    return new;
}


/*

*/
void message_player(char *message, Player *p)
{
    int numread;
    int count = 0, it = 0;

    while (message[it++] != '\0') {
        count++;
    }

    if ((numread = send(p->socket, message, count, 0)) == -1) {
            printf("server: error sending data packet. \n");
            exit(1);
    }
}

void send_JoinResult(char *res, Player *p){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "JoinResult");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "Name", p->name);
    cJSON_AddStringToObject(message, "Result", res);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(package), p);
    cJSON_Delete(package);
}

void send_Chat(char *text, char *sender, Player *p){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "Chat");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "Name", sender);
    cJSON_AddStringToObject(message, "Text", text);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(package), p);
    cJSON_Delete(package);
}

void send_StartInstance(Player *p, char *server, char *port){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "StartInstance");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "Server", server);
    cJSON_AddStringToObject(message, "Port", port);
    cJSON_AddNumberToObject(message, "Nonce", p->nonce);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(package), p);
    cJSON_Delete(package);
}

void send_JoinInstanceResult(char *res, Player *p){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "JoinInstanceResult");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "Name", p->name);
    cJSON_AddNumberToObject(message, "Number", p->num);
    cJSON_AddStringToObject(message, "Result", res);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(package), p);
    cJSON_Delete(package);
}

void send_StartGame(int rounds, Player **players, Player *p){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "StartGame");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddNumberToObject(message, "Rounds", rounds);
    add_player_array(message, players);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(package), p);
    cJSON_Delete(package);
}

void send_StartRound(int wordlen, int Round, int remain, Player **players, Player *p){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "StartRound");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddNumberToObject(message, "WordLength", wordlen);
    cJSON_AddNumberToObject(message, "Round", Round);
    cJSON_AddNumberToObject(message, "RountsRemaining", remain);
    add_player_array(message, players);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(package), p);
    cJSON_Delete(package);
}

void send_PromptForGuess(int wordlen, Player *p, int guessnum){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "PromptForGuess");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "Name", p->name);
    cJSON_AddNumberToObject(message, "GuessNumber", guessnum);
    cJSON_AddNumberToObject(message, "WordLength", wordlen);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(package), p);
    cJSON_Delete(package);
}

void send_GuessResponse(Player *p, char *guess, char *ac){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "GuestResponse");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "Name", p->name);
    cJSON_AddStringToObject(message, "Accepted", ac);
    cJSON_AddStringToObject(message, "Guess", guess);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(package), p);
    cJSON_Delete(package);
}

void send_GuessResult(Player *p, Player **ps, char *win){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "GuessResult");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "Winner", win);
    add_player_array_gr(message, ps);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(package), p);
    cJSON_Delete(package);
}

void send_EndRound(Player *p, Player **ps, int rem){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "EndRound");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddNumberToObject(message, "RoundsRemaining", rem);

    add_player_array_er(message, ps);
    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(package), p);
    cJSON_Delete(package);
}

void send_EndGame(Player *p, char *winner, Player **players){
    cJSON *package = cJSON_CreateObject();
    cJSON_AddStringToObject(package, "MessageType", "EndGame");

    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "WinnerName", winner);

    add_player_array(message, players);

    cJSON_AddItemToObject(package, "Data", message);

    message_player(cJSON_Print(package), p);
    cJSON_Delete(package);
}


cJSON *add_player_array(cJSON *json, Player **ps){
    cJSON *players = cJSON_AddArrayToObject(json, "PlayerInfo");

    for (int i = 0; i < PLAYERS; i++){
        cJSON *playerInfo = cJSON_CreateObject();
        cJSON_AddStringToObject(playerInfo, "Name", ps[i]->name);
        cJSON_AddNumberToObject(playerInfo, "Number", ps[i]->num);
        cJSON_AddNumberToObject(playerInfo, "Score", ps[i]->score);

        cJSON_AddItemToArray(players, playerInfo);
    }

    return json;
}

cJSON *add_player_array_er(cJSON *json, Player **ps){
    cJSON *players = cJSON_AddArrayToObject(json, "PlayerInfo");

    for (int i = 0; i < PLAYERS; i++){
        cJSON *playerInfo = cJSON_CreateObject();
        cJSON_AddStringToObject(playerInfo, "Name", ps[i]->name);
        cJSON_AddNumberToObject(playerInfo, "Number", ps[i]->num);
        cJSON_AddNumberToObject(playerInfo, "Score", ps[i]->score);
        cJSON_AddStringToObject(playerInfo, "Winner", ps[i]->winner);

        cJSON_AddItemToArray(players, playerInfo);
    }

    return json;
}

cJSON *add_player_array_gr(cJSON *json, Player **ps){
    cJSON *players = cJSON_AddArrayToObject(json, "PlayerInfo");

    for (int i = 0; i < PLAYERS; i++){
        cJSON *playerInfo = cJSON_CreateObject();
        cJSON_AddStringToObject(playerInfo, "Name", ps[i]->name);
        cJSON_AddNumberToObject(playerInfo, "Number", ps[i]->num);
        cJSON_AddStringToObject(playerInfo, "Correct", ps[i]->correct);
        cJSON_AddStringToObject(playerInfo, "ReceiptTime", ps[i]->rec_time);
        cJSON_AddStringToObject(playerInfo, "Result", ps[i]->res);

        cJSON_AddItemToArray(players, playerInfo);
    }

    return json;
}

Player *recv_Join(cJSON *message, int sock, int num, int nonce){
    cJSON *data = cJSON_GetObjectItemCaseSensitive(message, "Data");
    cJSON *name = cJSON_GetObjectItemCaseSensitive(data, "Name");
    Player *new = create_player(name->valuestring, sock, num, nonce);
    return new;
}

char *recv_Chat(cJSON *message){
    cJSON *data   = cJSON_GetObjectItemCaseSensitive(message, "Data");
    cJSON *j_name = cJSON_GetObjectItemCaseSensitive(data, "Name");
    cJSON *j_text = cJSON_GetObjectItemCaseSensitive(data, "Text");
    char *mes = (char *)calloc(SIZE, sizeof(char));
    sprintf(mes, "%s: %s\n", j_name->valuestring, j_text->valuestring);
    return mes;
}

Player *recv_JoinInstance(cJSON *message, int sock, int num){
    cJSON *data = cJSON_GetObjectItemCaseSensitive(message, "Data");
    cJSON *name = cJSON_GetObjectItemCaseSensitive(data, "Name");
    cJSON *nonce = cJSON_GetObjectItemCaseSensitive(data, "Nonce");
    Player *new = create_player(name->valuestring, sock, num, nonce->valueint);
    return new;
}

char *recv_Guess(cJSON *message, Player *p){
    cJSON *data = cJSON_GetObjectItemCaseSensitive(message, "Data");
    cJSON *guess = cJSON_GetObjectItemCaseSensitive(data, "Guess");
    return guess->valuestring;
}

Player *find_player(Player **ps, char *name){
    int i = 0;
    Player *p = ps[i];
    while (p){
        if (strcat(p[0].name, name) == 0){
            return p;
        }
        p = ps[++i];
    }
    return NULL;
}
