#ifndef Calendar_H
#define Calendar_H

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
    ADD, REMOVE, UPDATE, GET, GETALL, INPUTS
} RequestType;

typedef struct event event;
struct event {
    char *name;
    char *date;
    char *time;
    char *duration;
    char *location;
    char *identifier;
    event *next;
    event *prev;
};

typedef struct request request;
struct request {
    RequestType type;
    char *calName;
    event *event;
    char *eventName;
};

typedef struct Calendar Calendar;
struct Calendar {
    char *name;
    event *head;
    FILE *file;
    int count;
};

int free_calendar(Calendar *cal);

char *string_from_request(request *e);
request *request_from_string(char *s);
char *string_from_event(event *e);
event *event_from_string(char *s);

Calendar *add_event      (Calendar *cal, event *e);
int delete_calendar(Calendar *cal);
int remove_event   (Calendar *cal, char *event_id);
event** get_events_by_date(Calendar *cal, char* date);
event** get_events_by_range(Calendar *cal, char* start_date, char* end_date);
event *create_event(char *name, char *date, char *time, char *duration, char *location, char* identifier);
int in_date_range(char* start, char* end, char *date);

Calendar *load_calendar(char *file_path, char *name);
Calendar *process_edit_request(request *req, Calendar *cal);
int add_request(request *req, FILE *fp);

request *create_request(char *json_string);
int close_request(request *req);

#endif
