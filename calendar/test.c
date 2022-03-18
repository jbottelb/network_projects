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

    // Calendar *c = (Calendar *)malloc(sizeof(Calendar));
    Calendar *cal = load_calendar("data/JoeC", "JoeC");
    char s[BUFSIZ] = " \
    { \
        \"CALENDAR\": \"JoeC\", \
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
    printf("Cal init size: %d\n", cal->count);

    request * req = request_from_string(s);

    printf("%s, %s, %s, %s, %s", req->event->name, req->event->date, req->event->time,req->event-> duration, req->event->description);
    printf("\n%d\n", req->type);
    printf("\n%s\n", req->OG);

    cal = process_edit_request(req, cal);
    save_request(req, cal->file);

    char t[BUFSIZ] = " \
    { \
        \"CALENDAR\": \"JoeC\", \
        \"Action\": \"ADD\", \
        \"Arguments\": { \
            \"date\": 1203982, \
            \"time\": 1233, \
            \"duration\": \"indefinite\", \
            \"name\": \"Joe\", \
            \"description\": \"being mean\", \
            location: \"everywhere\" \
        }\
    } \
    ";
    request* req3 = request_from_string(t);
    cal = process_edit_request(req3, cal);
    save_request(req3, cal->file);

    return 0;
}
