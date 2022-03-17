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

typedef struct calendar calendar;
struct calendar {
    char *name;
    event *head;
};

char *string_from_calendar(calendar *cal);
char *string_from_event(event *e);
event *event_from_string(char *s);

int *add_event      (calendar *cal, event *e);
int *remove_event   (calendar *cal, char *event_id);
int *update_event   (calendar *cal, event *event_id);
event** get_events_by_date(calendar *cal, char* date);
event** get_events_by_range(calendar *cal, char* start_date, char* end_date);
