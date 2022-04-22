#include <stdio.h>
#include <stdlib.h>
#include "wordle.h"
#include "cJSON.h"

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
    cJSON *json = cJSON_Parse(Json);
    printf("%s\n", cJSON_Print(json));

    return 0;
}
