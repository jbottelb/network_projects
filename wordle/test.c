#include <stdio.h>
#include <stdlib.h>
#include "wordle.h"
#include "cJSON.h"
#include "player.h"
#include "server/server.h"

int main() {
    printf("BEGIN\n");

    Wordle *B = create_board("NAME", "HOARD", 6);
    char *res = make_guess("HAOFF", B);
    printf("RES: %s\n", res);
    printf("GS: %d\n", B->state);

    char key[10], value[10];
    sscanf("[{ key : value }]", "[{ %s : %s }]", key, value);
    printf("%s %s\n", key, value);
    char Json[BUFSIZ] = "{ \"key\" : \"value\"}";

    cJSON *v1 = cJSON_CreateString("A");
    cJSON *json = cJSON_Parse(Json);
    cJSON_AddItemToObject(json, "1", v1);
    cJSON_AddStringToObject(json, "2", "B");
    printf("%s\n", cJSON_Print(json));

    printf("This should be B : %s\n", cJSON_GetObjectItemCaseSensitive(json, "2")->valuestring);

    cJSON *structure = cJSON_CreateObject();
    cJSON_AddStringToObject(structure, "MT", "An MT");

    cJSON *data = cJSON_CreateObject();
    cJSON_AddStringToObject(data, "data", "items in data");
    cJSON_AddItemToObject(structure, "data", data);

    printf("%s\n", cJSON_Print(structure));
    cJSON_Delete(json);

    Player *p = (Player *)calloc(1, sizeof(Player));
    p->score = 10;
    p->nonce = 1;
    p->num = 0;
    char *name = (char *)calloc(1, sizeof(char));
    name = "Player0";
    p->name = name;
    char *resp = (char *)calloc(1, sizeof(char));
    resp = "bloop";
    p->res = resp;
    char *rec_time = (char *)calloc(1, sizeof(char));
    rec_time = "bloop";
    p->rec_time = rec_time;
    char *correct = (char *)calloc(1, sizeof(char));
    correct = "yes";
    p->correct = correct;
    char *winner = (char *)calloc(1, sizeof(char));
    winner = "no";
    p->winner = winner;

    Player *ps[1];
    ps[0] = p;
    printf("%s\n", ps[0]->name);

    // must add print statement to work
    //send_StartInstance(p, "localhost", "porthole");


    char *word = (char *)calloc(1, sizeof(char));
    word = "bxard";
    if (in_word_list(word) == 0){
        printf("Word in list\n");
    } else {
        printf("Word not in list\n");
    }

    Wordle *w = calloc(1, sizeof(Wordle));
    printf("%s\n", select_word(w));

    return 0;
}
