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
#include "server.h"
#include "calendar.h"

int main(){
    printf("\nComence Testing\n\n");

    char s[BUFSIZ] = " \
    { \
        \"CALENDAR\": \"Joe's Calendar\", \
        \"Action\": \"ADD\", \
        \"Arguments\": { \
            \"date\": 012222, \
            \"time\": 1233, \
            \"duration\": \"indefinite\", \
            \"name\": \"Joe\", \
            \"description\": \"being mean\", \
            location: \"everywhere\" \
        }\
    } \
    ";

    request * req = request_from_string(s);

    return 0;
}
