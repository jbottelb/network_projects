#ifndef WORDLE_H
#define WORDLE_H

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

#define MAXLETTERS 10
#define MAXGUESSES 10

typedef enum {
    EMPTY, WRONG, IN, CORRECT
} LetterType;

typedef enum {
    CREATED, STARTED, WON, LOST
} GameState;

typedef struct Wordle Wordle;
struct Wordle {
    char        *name;
    char        Letters[MAXGUESSES][MAXLETTERS];
    LetterType  Guesses[MAXGUESSES][MAXLETTERS];
    int         count;
    char       *word;
    int         wordlen;
    int         max_guesses;
    GameState   state;
};

Wordle *create_board (char *name, int guesses);

/*
    Updates board appropriately and returns the result of the guess
    Game state will be updated to WON if the guess was correct
*/
char *make_guess (char *guess, Wordle *board);
int is_correct (char *res);
int in_word_list(char *word);
char *select_word(Wordle *w);
int score_guess(char *res, int round);


void print_board(Wordle *b);
char *censor(char *str);

// helpers
int in_word(char *word, char c);

#endif
