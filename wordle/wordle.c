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
#include "wordle.h"

Wordle *create_board (char *name, char *word, int guesses){
    Wordle *board = (Wordle *)calloc(1, sizeof(Wordle));
    board->name = name;
    board->word = word;
    board->count = 0;
    board->state = CREATED;
    board->max_guesses = guesses;
    if (guesses){
        board->max_guesses = guesses;
    }
    int l;
    for (l = 0; word[l] != '\0'; ++l);
    board->wordlen = l;
    for (int i = 0; i < guesses; i++){
        for (int j = 0; j < l; j++){
            board->Guesses[i][j] = EMPTY;
            board->Letters[i][j] = ' ';
        }
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
    board->count++;
    if (won == 1){
        board->state = WON;
        return res;
    }
    board->state = STARTED;
    return res;
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
