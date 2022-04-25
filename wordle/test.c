#include <stdio.h>
#include <stdlib.h>
#include "wordle.h"
#include "cJSON.h"
#include "player.h"
#include "server/server.h"

int main() {
    printf("BEGIN\n");

    Wordle *B = create_board("NAME", 6);
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

    Player *p = (Player *)calloc(128, sizeof(Player));
    p->score = 10;
    p->nonce = 1;
    p->num = 0;
    char *name = (char *)calloc(128, sizeof(char));
    name = "Player0";
    p->name = name;
    char *resp = (char *)calloc(128, sizeof(char));
    resp = "bloop";
    p->res = resp;
    char *rec_time = (char *)calloc(128, sizeof(char));
    rec_time = "bloop";
    p->rec_time = rec_time;
    char *correct = (char *)calloc(128, sizeof(char));
    correct = "yes";
    p->correct = correct;
    char *winner = (char *)calloc(128, sizeof(char));
    winner = "no";
    p->winner = winner;

    Player *ps[1];
    ps[0] = p;
    printf("%s\n", ps[0]->name);

    // must add print statement to work
    //send_StartInstance(p, "localhost", "porthole");


    char *word = (char *)calloc(128, sizeof(char));
    word = "bxard";
    if (in_word_list(word) == 0){
        printf("Word in list\n");
    } else {
        printf("Word not in list\n");
    }

    Wordle *w = calloc(1, sizeof(Wordle));
    char *mword = select_word(w);

    Wordle *mB = create_board("NAME", 6);

    // Lets play a game
    printf("Play Game\n");
    printf("Cheat: %s\n", mword);
    Player *me = (Player *)calloc(1, sizeof(Player));
    me->winner = "no";

    me->score = 0;
    while (strstr("yes", me->winner) == 0 && 1 == 0){
        printf("Guess\n");
        char my_guess[5];
        scanf("%s", my_guess);
        if (in_word_list(my_guess) == 1){
            printf("Not in word list\n");
            continue;
        }
        char *res = make_guess(my_guess, mB);
        mB->count++;
        printf("Result: %s\n", res);
        printf("Remaining rounds: %d\n", mB->max_guesses - mB->count);
        me->score += score_guess(res, mB->count);
        printf("Round Score: %d\n",score_guess(res, mB->count));
        if (is_correct(res) == 0){
            printf("You won the game with in %d rounds!\n", mB->count);
            me->winner = "yes";
            break;
        }
        if (mB->max_guesses - mB->count == 0){
            printf("You did not get the word.\n");
            break;
        }
    }
    printf("Total Score: %d\n", me->score);

    char *str = "{\"MessageType\" : \"Nope\", \"Data\" : { \"Name\": \"Josh\", \"Text\" : \"I like trains\"}}";
    printf("%s\n", str);
    cJSON *item = cJSON_Parse(str);
    printf("%s\n", cJSON_Print(item));
    char *str4 = recv_Chat(item);
    printf("%s", str4);

    char *sc = (char *)calloc(128, sizeof(char));
    sc = "YGBBB";
    printf("Score: %d\n", score_guess(sc, 2));

    printf("Censored word: %s\n", censor("applefuck"));

    char *jh = (char *)calloc(128, sizeof(char));
    char *je = (char *)calloc(128, sizeof(char));
    jh = "Josh";
    je = "Joe";
    Player *p1 = create_player(je, 0, 0, 0);
    Player *p2 = create_player(jh, 0, 0, 0);

    Player **pls = calloc(16, sizeof(Player *));
    pls[0] = p1;
    pls[1] = p2;
    printf("%s\n", pls[1]->name);
    Player *found = find_player(pls, "Josh");
    if (found){
        printf("Found Player: %s\n", found->name);
    } else {
        printf("%s\n", "No found player");
    }

    return 0;
}
