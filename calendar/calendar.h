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

typedef struct Calendar Calendar;
struct Calendar {
    char *name;
    event *head;
    int count;
};

char *string_from_Calendar(Calendar *cal);
char *string_from_event(event *e);
event *event_from_string(char *s);

Calendar *add_event      (Calendar *cal, event *e);
int remove_event   (Calendar *cal, char *event_id);
event** get_events_by_date(Calendar *cal, char* date);
event** get_events_by_range(Calendar *cal, char* start_date, char* end_date);
event *create_event(char *name, char *date,
    char *time, char *duration, char *location, char* identifier);
// used to find dates in a range for range lookup
int in_date_range(char* start, char* end, char *date);

#endif
