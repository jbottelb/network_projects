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

#include <glib-object.h>
#include <json-glib/json-glib.h>

char *string_from_calendar(calendar *cal)
{
    return NULL;
}

char *string_from_event(event *e)
{
    return NULL;
}

event *event_from_string(char *s)
{
    return NULL;
}

int *add_event(calendar *cal, event *e)
{
    return NULL;
}

int *remove_event(calendar *cal, char *event_id)
{
    return NULL;
}

int *update_event(calendar *cal, event *event_id)
{
    return NULL;
}

event** get_events_by_date(calendar *cal, char* date)
{
    return NULL;
}

event** get_events_by_range(calendar *cal, char* start_date, char* end_date)
{
    return NULL;
}
