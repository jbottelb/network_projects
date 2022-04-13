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

typedef enum {
    EMPTY, NOT_IN, IN, CORRECT
} LetterType;

typedef struct Board Board;
struct Board {
    char        *name;
    char        Letters[6][5];
    LetterType  Guesses[6][5];
};

Board *create_board (char *name);

Board *make_guess (char *guess);

#endif
