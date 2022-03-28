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
    char  *name;
    char  *date;
    char  *time;
    char  *duration;
    char  *location;
    char  *description;
    int    identifier;
    event *next;
    event *prev;
};

typedef struct request request;
struct request {
    RequestType type;
    char  *OG;           // origonal string for storage
    char  *calName;
    char  *param;        // not used in ADD, for range, start_date:end_date
    event *event;       // only used in ADD
};

typedef struct Calendar Calendar;
struct Calendar {
    char  *name;
    event *head;
    char  *file_path;
    int    identifier_counter;
    FILE  *fp;
    int    count;
};
// let the pretty spacing decieve you into thinking this is organized

char     *get_single_arg        (char *str);
char     *get_double_arg        (char *str);
char     *get_tripple_arg       (char *str);
int       in_date_range         (char* start, char* end, char *date);

Calendar *process_edit_request  (request *req, Calendar *cal);
event    *create_event          (char *name, char *date, char *time, char *duration, char *location, int identifier);
Calendar *add_event             (Calendar *cal, event *e);
int       remove_event          (Calendar *cal, char *i_string);
int       free_event            (event *e);
void      update_event          (Calendar *cal, char *params);
int       delete_calendar       (Calendar *cal);
event  ** get_events_by_date    (Calendar *cal, char* date);
event  ** get_events_by_range   (Calendar *cal, char* params);

void      dump_calendar         (Calendar *cal);
Calendar *load_calendar         (char *file_path, char *name);
int       free_calendar         (Calendar *cal);

char     *string_from_event     (event *e);
event    *event_from_string     (char  *s  );
request  *request_from_string   (char  *s  );
int       save_request          (request *req, Calendar *fp);
void      close_request         (request *req);
RequestType get_request_type    (char* reqType);

#endif