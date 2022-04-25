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
#include <time.h>
#include <math.h>
#include "wordle.h"

Wordle *create_board (char *name, int guesses){
    Wordle *board = (Wordle *)calloc(1, sizeof(Wordle));
    board->name = name;
    board->count = 0;
    board->state = CREATED;
    board->max_guesses = guesses;
    if (guesses){
        board->max_guesses = guesses;
    }
    return board;
}

char *make_guess (char *guess, Wordle *board){
    int l = board->wordlen;
    int won = 0;
    char *res = calloc(l, sizeof(char));
    for (int i = 0; i < l; i++){
        char actual = board->word[i];
        if (!actual) break;
        if (guess[i] == actual){
            board->Letters[board->count][i] = CORRECT;
            res[i] = 'G';
            won = 1;
        } else if (in_word(board->word, guess[i])) {
            board->Letters[board->count][i] = IN;
            res[i] = 'Y';
            won = 0;
        } else {
            board->Guesses[board->count][i] = WRONG;
            res[i] = 'B';
            won = 0;
        }
        board->Letters[board->count][i] = guess[i];
    }
    if (won == 1){
        board->state = WON;
        return res;
    }
    board->state = STARTED;
    return res;
}

char *select_word(Wordle *w){
    int num = 2314; // number of words in file
    char *word = (char *) calloc(5, sizeof(char));
    time_t t;
    int index;
    srand((unsigned) time(&t));
    index = rand() % num;
    // now we move forward that many spaces, then select that word
    char string[5];
    FILE *file = fopen("server/word_list.txt", "r");
    while ( fscanf(file, "%s", string) ){
        int i = 0;
        if (index == 0){
            int k = 0;
            while (string[k] != '\0'){
                word[k] = string[k];
                k++;
            }
            w->wordlen = k;
            w->word = word;
            fclose(file);
            return word;
        }
        index--;
        i++;
    }
    fclose(file);
    return word;
}

int in_word_list(char *word){
    char string[50];
    FILE *in_file = fopen("server/word_list.txt", "r");
    while ( fscanf(in_file,"%s", string) == 1){
        if(strstr(string, word) != 0) {
            fclose(in_file);
            return 0;
        }
    }
    fclose(in_file);
    return 1;
}


int is_correct (char *res){
    int i = 0;
    while (res[i] != '\0'){
        if (res[i++] != 'G'){
            return 1;
        }
    }
    return 0;
}

int in_word(char *word, char c){
    for (int i = 0; i < 10; i++){
        if (!word[i]){
            return 0;
        }
        if (word[i] == c){
            return 1;
        }
    }
    return 0;
}

int score_guess(char *res, int r){
    double mult = (1 / ((double)r * (double)r)) * 100;
    int score = 0;
    int c = 0;
    if (is_correct(res) == 0){
        return trunc(3500 * 1 / ((double)r * (double)r));
    }
    // round 1 is luck, unless you got it right, just a couple points
    // 10, just to make sure we correctly gave you a score
    if (r == 1){
        return 10;
    }
    while (res[c] != '\0'){
        if (res[c] == 'G'){
            score += 5;
        } else if (res[c] == 'Y'){
            score += 2;
        }
        c++;
    }
    return trunc(mult * score);;
}

// not actually needed
void print_board(Wordle *b){
    printf("Word length: %d Max Guesses: %d\n", b->wordlen, b->max_guesses);
    printf("GUESSES\n");
    for (int i = 0; i < b->wordlen; i++){
        for (int j = 0; j < b->max_guesses; j++){
            printf("%c ", b->Letters[i][j]);
        }
        printf("\n");
    }
    printf("\nCORRECTNESS\n");
    for (int i = 0; i < b->wordlen; i++){
        for (int j = 0; j < b->max_guesses; j++){
            printf("%d ", b->Guesses[i][j]);
        }
        printf("\n");
    }
}

char *censor(char *str){
    char string[50];
    FILE *in_file = fopen("server/bad_words.txt", "r");
    while ( fscanf(in_file,"%s", string) == 1){
        if(strstr(str, string) != 0) {
            fclose(in_file);
            return "Bleep";
        }
    }
    fclose(in_file);
    return str;
}
