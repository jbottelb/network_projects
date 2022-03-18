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
#include "calendar.h"

request *request_from_string(char *s)
{
    request *req = (request*) malloc(sizeof(request));
    req->OG = s;

    int l = strlen(s);
    char buf[BUFSIZ];
    int i = 0, j=0;
    int inString = 0;

    // reduce to simplier format (no brackets or spacing)
    while (s[i] != '\0'){
        if (s[i] == '"'){
            inString = (inString == 1) ? 0 : 1;
            i++;
            continue;
        }
        if ((s[i] != ' ' && s[i] != '{' && s[i] != '}') || inString == 1){
            buf[j] = s[i];
            j++;
        }
        i++;
    }
    // get calendar name
    char *calName = malloc(BUFSIZ * sizeof(char));
    int it = 0, jt = 0;
    while (buf[it] != ':') {it++;}
    it++;
    while (buf[it] != ','){
        calName[jt] = buf[it];
        jt++; it++;
    }
    req->calName = calName;

    // get request type
    char *reqType = malloc(BUFSIZ * sizeof(char));
    while (buf[it] != ':') {it++;}
    it++;
    jt = 0;
    while (buf[it] != ','){
        reqType[jt] = buf[it];
        jt++; it++;
    }
    req->type = get_request_type(reqType);

    if (req->type == -1){
        printf("\nFailed to identify request type\n");
    }

    if (req->type == ADD){
        printf("Request being built as an ADD\n");
        while (buf[it] != ':') {it++;}
        it++;
        jt = 0;
        char *es = malloc(sizeof(char) * BUFSIZ);
        while (buf[it] != '\0')
        {
            es[jt] = buf[it];
            it++; jt++;
        }
        req->event = event_from_string(es);
    } else if (req->type == REMOVE) {
        printf("Request being built as a REMOVE\n");

    } else if (req->type == UPDATE) {
        printf("Request being built as an UPDATE\n");

    } else if (req->type == GET)    {
        printf("Request being built as a GET\n");

    } else if (req->type == GETALL) {
        printf("Request being built as an GETALL\n");

    } else if (req->type == INPUTS) {
        printf("Request being built as a INPUTS\n");

    }

    return req;
}

RequestType get_request_type(char* reqType){
    RequestType tr;
    if      (strcmp(reqType, "ADD")    == 0) {
        tr = ADD;
    } else if (strcmp(reqType, "REMOVE") == 0) {
        tr = REMOVE;
    } else if (strcmp(reqType, "UPDATE") == 0) {
        tr = UPDATE;
    } else if (strcmp(reqType, "GET"   ) == 0) {
        tr = GET;
    } else if (strcmp(reqType, "GETALL") == 0) {
        tr = GETALL;
    } else if (strcmp(reqType, "INPUTS") == 0) {
        tr = INPUTS;
    } else {
        tr = -1;
    }
    free(reqType);
    return tr;
}

event *event_from_string(char *s)
{
    event *e = (event *) malloc (sizeof(event));

    char *date = malloc(BUFSIZ * sizeof(char));
    int it = 0, jt = 0;
    while (s[it] != ':') {it++;}
    it++;
    while (s[it] != ','){
        date[jt] = s[it];
        jt++; it++;
    }
    e->date = date;
    char *time = malloc(BUFSIZ * sizeof(char));
    jt = 0;
    while (s[it] != ':') {it++;}
    it++;
    while (s[it] != ','){
        time[jt] = s[it];
        jt++; it++;
    }
    e->time = time;
    char *duration = malloc(BUFSIZ * sizeof(char));
    jt = 0;
    while (s[it] != ':') {it++;}
    it++;
    while (s[it] != ','){
        duration[jt] = s[it];
        jt++; it++;
    }
    e->duration = duration;
    char *name = malloc(BUFSIZ * sizeof(char));
    jt = 0;
    while (s[it] != ':') {it++;}
    it++;
    while (s[it] != ','){
        name[jt] = s[it];
        jt++; it++;
    }
    e->name = name;
    char *description = malloc(BUFSIZ * sizeof(char));
    jt = 0;
    while (s[it] != ':') {it++;}
    it++;
    while (s[it] != ','){
        description[jt] = s[it];
        jt++; it++;
    }
    e->description = description;
    char *location = malloc(BUFSIZ * sizeof(char));
    jt = 0;
    while (s[it] != ':') {it++;}
    it++;
    while (s[it] != '\0'){
        location[jt] = s[it];
        jt++; it++;
    }
    e->location = location;

    return e;
}

Calendar *add_event(Calendar *cal, event *e)
{
    if (!cal->head)
    {
        cal->head = e;
        cal->count++;
        return cal;
    }
    event *ptr = cal->head;
    while (ptr->next) {
        if (ptr->name == e->name)
            return NULL;
        ptr = ptr->next;
    }
    ptr->next = e;
    e->prev = ptr;
    cal->count++;
    return cal;
}

event *create_event(char *name, char *date, char *time,
    char *duration, char *location, char* identifier)
{
    if (!(name && date && time && duration && location && identifier))
        return NULL;
    event *new_event = malloc(sizeof(event));
    new_event->name = name;
    new_event->date = date;
    new_event->time = time;
    new_event->duration = duration;
    new_event->location = location;
    new_event->identifier = identifier;
    return new_event;
}

int remove_event(Calendar *cal, char *event_name)
{
    event *ptr = cal->head;
    while (strcmp(ptr->name,event_name)!=0);
    if (!ptr)
        return 1;
    event *temp = ptr->next;
    ptr->prev->next = ptr->next;
    if (ptr->next)
        ptr->next->prev = ptr->prev;
    free_event(ptr);
    cal->count--;
    return 0;
}

// Rememver to free the event array!
event** get_events_by_date(Calendar *cal, char* date)
{
    /*
    First, find how many events we have, then create an malloc an event array to
    hold them, then add them. That's C for ya
    */
    int count = 0;
    event *ptr = cal->head;
    // No events
    if (!ptr)
        return NULL;
    // go though and find events that count
    while (ptr->next){
        if (strcmp(ptr->name, date)){
            count++;
        }
    }
    // we found none
    if (count == 0)
        return NULL;
    int index = 0;
    // create array for events (should be FREEED!)
    event **events = (event**)malloc(count * sizeof(event));
    while (ptr->next){
        if (strcmp(ptr->name, date)){
            // add our event
            events[index] = ptr;
            index++;
        }
    }
    return events;
}

event** get_events_by_range(Calendar *cal, char* start_date, char* end_date)
{
    /*
    Literally just the same as the by date, but with the helper
    */
    int count = 0;
    event *ptr = cal->head;
    // No events
    if (!ptr)
        return NULL;
    // go though and find events that count
    while (ptr->next){
        if (in_date_range(start_date, end_date, ptr->date)){
            count++;
        }
    }
    // we found none
    if (count == 0)
        return NULL;
    int index = 0;
    // create array for events (should be FREEED!)
    event ** events = (event**)malloc(count * sizeof(event));
    while (ptr->next){
        if (in_date_range(start_date, end_date, ptr->date)){
            // add our event
            events[index] = ptr;
            index++;
        }
    }
    return events;
}

int in_date_range(char* start, char* end, char *date)
{
    /*
        Dates are 6 numbers: month, day, year
        (if you follow the layers of abstractions far enough,
            the code will be trash)
    */
    // within year range
    if (!(date[4] >= start[4] && date[5] >= start[5] &&
          date[4] <= end[4]   && date[5] <= end[5]   )){
        return 1;
    }
    // month range
    if (!(date[2] >= start[2] && date[3] >= start[3] &&
          date[2] <= end[2]   && date[3] <= end[3]   )){
        return 1;
    }
    // day range
    if (!(date[0] >= start[0] && date[1] >= start[1] &&
          date[0] <= end[0]   && date[1] <= end[1]   )){
        return 1;
    }
    return 0;
}

int is_loaded(Calendar *cal){
    if (cal->file){
        return 1;
    }
    return 0;
}

int free_event(event *e){
    free(e->name);
    free(e->date);
    free(e->time);
    free(e->location);
    free(e->duration);
    free(e->identifier);
    free(e);
}

int delete_calendar(Calendar *cal)
{
    event *ptr = cal->head;
    while (ptr){
        event *del = ptr;
        free_event(del);
        ptr = ptr->next;
    }
    free(cal->name);
    fclose(cal->file);
    free(cal);
    return 0;
}

/*

The following code is for dealing with the data on disk (in /data)

Each calendar is in its own file, and is loaded into a linked list of events
called a calendar. It is a linked list because that is the easist DS for me
to impliment in C. There is no other reason.

*/
Calendar *load_calendar(char *file_path, char *name)
{
    FILE *fp = fopen(file_path, "ra");


    Calendar *cal = (Calendar *)malloc(sizeof(Calendar));
    cal->name = name;
    cal->file = fp;
    cal->count = 0;
    cal->head = NULL;

    if (!fp) {
        printf("Creating file for calendar\n");
        fp = fopen(file_path, "wa");
        if (!fp) {
            printf("Failed to create file\n");
            return NULL;
        }
        cal->file = fp;
        return cal;
    } cal->file = fp;

    char *string = NULL;
    size_t read;
    size_t len = 0;
    while ((read = getline(&string, &len, fp)) != -1)
    {
        printf("Adding request from file %s to calendar %s:\n", file_path, name);
        printf("%s", string);

        request *req = request_from_string(string);

        process_edit_request(req, cal);

        free(string);
        free(req);
    }

    return cal;
}

/*
    Takes a raw string request and adds it to a calendar
    (used primarily for load operation)
*/
Calendar* process_edit_request(request *req, Calendar *cal)
{
    // add event
    switch (req->type){
        case ADD:
            add_event(cal, req->event);
        case REMOVE:
            break;
        case UPDATE:
            break;
        defualt:
            break;
    }


    return cal;
}

int save_request(request *req, FILE *fp)
{
    // Adds request string to the file
    fputs(req->OG, fp);
    fflush(fp);
    // free(req->OG);
    return 0;
}
