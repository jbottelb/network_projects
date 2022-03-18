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
    char *description;
    char *identifier;
    event *next;
    event *prev;
};

typedef struct request request;
struct request {
    RequestType type;
    char *OG;           // origonal string for storage
    char *calName;
    char **params;      // not used in ADD
    event *event;       // only used in ADD
};

typedef struct Calendar Calendar;
struct Calendar {
    char *name;
    event *head;
    char *file_path;
    FILE *fp;
    int count;
};

int free_calendar(Calendar *cal);
int free_event(event *e);
request *request_from_string (char  *s  );
event   *event_from_string   (char  *s  );


void dump_calendar(Calendar *cal);
Calendar *add_event      (Calendar *cal, event *e);
int       delete_calendar(Calendar *cal);
int       remove_event   (Calendar *cal, char *event_id);
event  ** get_events_by_date(Calendar *cal, char* date);
event  ** get_events_by_range(Calendar *cal, char* start_date, char* end_date);
event    *create_event(char *name, char *date, char *time, char *duration, char *location, char* identifier);
int       in_date_range(char* start, char* end, char *date);

Calendar *load_calendar(char *file_path, char *name);
Calendar *process_edit_request(request *req, Calendar *cal);
int       save_request(request *req, Calendar *fp);

request *create_request(char *json_string);
int      close_request(request *req);

RequestType get_request_type(char* reqType);

#endif
