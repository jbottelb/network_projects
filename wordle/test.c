#include <stdio.h>
#include <stdlib.h>
#include "wordle.h"

int main() {
    printf("BEGIN\n");

    Wordle *B = create_board("NAME", "WORDE", 6);
    char *res = make_guess("WEZZZ", B);
    printf("RES: %s\n", res);
    printf("GS: %d\n", B->state);

    char key[10], value[10];
    sscanf("[{ key : value }]", "[{ %s : %s }]", key, value);
    printf("%s %s\n", key, value);

    return 0;
}
