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
    char *path = (char *)malloc(BUFSIZ * sizeof(char));
    path = "data/JoeC";
    Calendar *cal = load_calendar(path, "JoeC");
    printf("\nCal init size: %d\n", cal->count);
    char s[BUFSIZ] = " \
    { \
        \"CALENDAR\": \"JoeC\", \
        \"Action\": \"ADD\", \
        \"Arguments\": { \
            \"date\": 012222, \
            \"time\": 1233, \
            \"duration\": \"indefinite\", \
            \"name\": \"BD\", \
            \"description\": \"being mean\", \
            location: \"everywhere\" \
        }\
    } \
    ";


    request * req = request_from_string(s);

    printf("%s, %s, %s, %s, %s\n", req->event->name, req->event->date, req->event->time,req->event-> duration, req->event->description);

    cal = process_edit_request(req, cal);
    save_request(req, cal);

    char t[BUFSIZ] = " \
    { \
        \"CALENDAR\": \"JoeC\", \
        \"Action\": \"ADD\", \
        \"Arguments\": { \
            \"date\": 1203982, \
            \"time\": 1233, \
            \"duration\": \"indefinite\", \
            \"name\": \"YEAD\", \
            \"description\": \"being mean\", \
            location: \"everywhere\" \
        }\
    } \
    ";
    request* req3 = request_from_string(t);
    cal = process_edit_request(req3, cal);
    save_request(req3, cal);

    dump_calendar(cal);

    return 0;
}
