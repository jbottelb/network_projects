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
    request *req = (request*) calloc(1, sizeof(request));
    req->OG = s;

    // printf("%s\n", s);

    int l = strlen(s);
    char buf[BUFSIZ] = {0}; // you would not believe how important the {0} is
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
    char *calName = (char *)calloc(BUFSIZ, sizeof(char));
    int it = 0, jt = 0;
    while (buf[it] != ':') {it++;}
    it++;
    while (buf[it] != ','){
        calName[jt] = buf[it];
        jt++; it++;
    }
    req->calName = calName;

    // get request type
    char *reqType = (char *)calloc(BUFSIZ, sizeof(char));

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
    // remaining string is passed to corresponding function for rest of params
    while (buf[it] != ':') {it++;}
    it++;
    jt = 0;
    char *arg_string = (char *)calloc(BUFSIZ, sizeof(char));
    while (buf[it] != '\0')
    {
        arg_string[jt] = buf[it];
        it++; jt++;
    }

    if (req->type == ADD){
        printf("Request being built as an ADD\n");
        req->event = event_from_string(arg_string);
    } else if (req->type == REMOVE) {
        printf("Request being built as a REMOVE\n");
        // in this case, the request param will be set to event ident
        req->param = get_single_arg(arg_string);
    } else if (req->type == UPDATE) {
        printf("Request being built as an UPDATE\n");
        // build param as identifier:feild:value
        req->param = get_tripple_arg(arg_string);
    } else if (req->type == GET)    {
        printf("Request being built as a GET\n");
        req->param = get_single_arg(arg_string);
    } else if (req->type == GETALL) {
        printf("Request being built as an GETALL\n");
        // set param to start_date:end_date
        req->param = get_double_arg(arg_string);
    } else if (req->type == INPUTS) {
        printf("Request being built as a INPUTS\n");
        // put all the inputs into the param
        req->param = arg_string;
        // this case we don't free our string
        return req;
    }
    free(arg_string);
    return req;
}

char *get_tripple_arg(char *str){
    // used for start_date end_date
    // generates ident:feild:value
    char *params = (char *)calloc(BUFSIZ, sizeof(char));
    int it = 0, jt = 0;
    while (str[it++] != ':');
    while (str[it] != ','){
        params[jt++] = str[it++];
    } it ++;
    params[jt++] = ':';
    while (str[it] != ':'){
        params[jt++] = str[it++];
    }
    while (str[it] != '\0'){
        params[jt++] = str[it++];
    }
    return params;
}

char *get_double_arg(char *str){
    // used for start_date end_date
    char *params = (char *)calloc(BUFSIZ, sizeof(char));
    int it = 0, jt = 0;
    while (str[it++] != ':');
    while (str[it] != ','){
        params[jt++] = str[it++];
    }
    params[jt++] = ':';
    while (str[it++] != ':');
    while (str[it] != '\0'){
        params[jt++] = str[it++];
    }
    return params;
}

char *get_single_arg(char *str){
    // if there is only one arg for the request, we can use this
    char *param = (char *)calloc(BUFSIZ, sizeof(char));
    int it = 0, jt = 0;
    while (str[it] != ':') {it++;}
    it++;
    while (str[it] != '\0'){
        param[jt] = str[it];
        jt++; it++;
    }
    return param;
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
        printf("Invalid type, got: %s\n", reqType);
        tr = -1;
    }
    free(reqType);
    return tr;
}

event *event_from_string(char *s)
{
    event *e = (event *) calloc (1, sizeof(event));

    char *date = (char *)calloc(BUFSIZ, sizeof(char));
    int it = 0, jt = 0;
    while (s[it] != ':') {it++;}
    it++;
    while (s[it] != ','){
        date[jt] = s[it];
        jt++; it++;
    }
    e->date = date;
    char *time = (char *)calloc(BUFSIZ, sizeof(char));
    jt = 0;
    while (s[it] != ':') {it++;}
    it++;
    while (s[it] != ','){
        time[jt] = s[it];
        jt++; it++;
    }
    e->time = time;
    char *duration = (char *)calloc(BUFSIZ, sizeof(char));
    jt = 0;
    while (s[it] != ':') {it++;}
    it++;
    while (s[it] != ','){
        duration[jt] = s[it];
        jt++; it++;
    }
    e->duration = duration;
    char *name = (char *)calloc(BUFSIZ, sizeof(char));
    jt = 0;
    while (s[it] != ':') {it++;}
    it++;
    while (s[it] != ','){
        name[jt] = s[it];
        jt++; it++;
    }
    e->name = name;
    char *description = (char *)calloc(BUFSIZ, sizeof(char));
    jt = 0;
    while (s[it] != ':') {it++;}
    it++;
    while (s[it] != ','){
        description[jt] = s[it];
        jt++; it++;
    }
    e->description = description;
    char *location = (char *)calloc(BUFSIZ, sizeof(char));
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

char *string_from_event(event *e){
    char *str = (char *)calloc(BUFSIZ, sizeof(char));
    // STRCAT is dangerous, so if there is an issue, it may be here.
    strcat(str, "{ \"date\": \"");
    strcat(str, e->date);
    strcat(str, "\", \"time\": \"");
    strcat(str, e->time);
    strcat(str, "\", \"duration\": \"");
    strcat(str, e->duration);
    strcat(str, "\", \"name\": \"");
    strcat(str, e->name);
    strcat(str, "\", \"description\": \"");
    strcat(str, e->description);
    strcat(str, "\", \"location\": \"");
    strcat(str, e->location);
    strcat(str, "\", \"identifier\": \"");
    char buf[BUFSIZ] = {0};
    sprintf(buf, "%d", e->identifier);
    strcat(str, buf);
    strcat(str, "\" }");
    return str;
}

Calendar *add_event(Calendar *cal, event *e)
{
    e->identifier = cal->identifier_counter++;
    printf("Assigning identifier to event: %d\n", e->identifier);
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
    char *duration, char *location, int identifier)
{
    if (!(name && date && time && duration && location && identifier))
        return NULL;
    event *new_event = (event *)calloc(1, sizeof(event));
    new_event->name = name;
    new_event->date = date;
    new_event->time = time;
    new_event->duration = duration;
    new_event->location = location;
    new_event->identifier = identifier;
    return new_event;
}

int remove_event(Calendar *cal, char *i_string)
{
    int identifier = atoi(i_string);
    free(i_string);
    printf("Remove int: %d\n", identifier);
    event *ptr = cal->head;
    if (ptr->prev){
        printf("Head note has a prev. That is bad\n");
    }

    while (ptr->identifier!=identifier){
        if (!ptr->next)
        {
            printf("No node by identifer %d\n", identifier);
            return 1;
        }
        ptr = ptr->next;
    }
    printf("%s\n", ptr->name);

    if (ptr == cal->head){
        cal->head = ptr->next;
        if (ptr->next)
            ptr->next->prev = NULL;
    }
    else {
        if (!ptr->next){
            ptr->prev->next = NULL;
        } else {
            ptr->next->prev = ptr->prev;
            ptr->prev->next = ptr->next;
        }
    }
    free_event(ptr);
    cal->count--;
    printf("Removed event successfully. Calendar size: %d\n", cal->count);
    return 0;
}

// Rememver to free the event array!
event** get_events_by_date(Calendar *cal, char* date)
{
    /*
    First, find how many events we have, then create an malloc an event array to
    hold them, then add them. That's C for ya [maybe im just bads]
    */
    int count = 0;
    event *ptr = cal->head;
    // No events
    if (!ptr)
        return NULL;
    // go though and find events that count
    while (ptr){
        if (strcmp(ptr->date, date) == 0){
            printf("Match: %s %s\n", ptr->date, date);
            count++;
        }
        ptr = ptr->next;
    }
    printf("Events found: %d\n", count);
    // we found none
    if (count == 0)
        return NULL;
    int index = 0;
    // create array for events (should be FREEED!)
    event **events = (event**)calloc(count, sizeof(event));
    ptr = cal->head;
    while (ptr){
        if (strcmp(ptr->date, date) == 0){
            // add our event
            events[index] = ptr;
            index++;
        }
        ptr = ptr->next;
    }
    return events;
}

event** get_events_by_range(Calendar *cal, char* params)
{
    // break down into start and end
    char start_date[BUFSIZ] = {0};
    char end_date[BUFSIZ] = {0};
    int it = 0, jt = 0;
    while (params[it] != ':'){
        start_date[jt++] = params[it++];
    } it++; jt = 0;

    while (params[it] != '\0'){
        end_date[jt++] = params[it++];
    }
    /*
    Literally just the same as the by date, but with the helper
    */
    int count = 0;
    event *ptr = cal->head;
    // No events
    if (!ptr)
        return NULL;
    // go though and find events that count
    while (ptr){
        if (in_date_range(start_date, end_date,ptr->date) == 0){
            printf("Match: %s %s\n", ptr->date, ptr->date);
            count++;
        }
        ptr = ptr->next;
    }
    printf("Events found: %d\n", count);
    // we found none
    if (count == 0)
        return NULL;
    int index = 0;
    // create array for events (should be FREEED!)
    event **events = (event**)calloc(count, sizeof(event));
    ptr = cal->head;
    while (ptr){
        if (in_date_range(start_date, end_date, ptr->date) == 0){
            // add our event
            events[index] = ptr;
            index++;
        }
        ptr = ptr->next;
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

int free_event(event *e){
    free(e->name);
    free(e->date);
    free(e->time);
    free(e->location);
    free(e->duration);
    free(e);
    return 0;
}

int delete_calendar(Calendar *cal)
{
    event *ptr = cal->head;
    while (ptr){
        event *del = ptr;
        ptr = ptr->next;
        free_event(del);
    }
    fclose(cal->fp);
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
    if (!fp){
        printf("Failed to open file\n");
    }

    Calendar *cal = (Calendar *)calloc(1, sizeof(Calendar));
    cal->file_path = file_path;
    cal->name = name;
    cal->count = 0;
    cal->head = NULL;
    cal->identifier_counter = 0;

    if (!fp) {
        printf("Creating file for calendar\n");
        fp = fopen(file_path, "a");
        if (!fp) {
            printf("Failed to create file\n");
            return NULL;
        }
        return cal;
    }

    char *string = NULL;
    size_t read;
    size_t len = 0;
    while ((read = getline(&string, &len, fp)) != -1)
    {
        printf("Adding request from file %s to calendar %s:\n", file_path, name);

        request *req = request_from_string(string);
        process_edit_request(req, cal);
        close_request(req);
    }
    printf("Done reading from file");

    return cal;
}

void dump_calendar(Calendar *cal){
    event *curr = cal->head;
    printf("Calendar Contents: \n");
    if (!curr){
        printf("Empty Calendar\n");
        return;
    }
    while (curr){
        printf("NAME: %s TIME: %s \n", curr->name, curr->time);
        curr=curr->next;
    }
    return;
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
            break;
        case REMOVE:
            remove_event(cal, req->param);
            break;
        case UPDATE:
            update_event(cal, req->param);
            break;
        defualt:
            break;
    }

    return cal;
}

void update_event(Calendar *cal, char *params){
    // identifier:feild:value
    // use that to find and change the requested value
    char identifier[BUFSIZ] = {0};
    char feild[BUFSIZ] = {0};
    // this is the only one that persists
    char *value = (char*)calloc(BUFSIZ, sizeof(char));

    int it = 0, jt = 0;
    while (params[it] != ':'){
        identifier[jt++] = params[it++];
    } it++; jt = 0;
    while (params[it] != ':'){
        feild[jt++] = params[it++];
    } it++; jt = 0;
    while (params[it] != '\0'){
        value[jt++] = params[it++];
    }
    printf("%s %s %s\n", identifier, feild, value);
    int id = atoi(identifier);

    // find event
    event *ptr = cal->head;
    while (ptr){
        if (ptr->identifier == id){
            printf("Found the event to update\n");
            if (strcmp("date", feild) == 0){
                ptr->date = value;
            } else if (strcmp("time", feild) == 0)       {
                ptr->time = value;
            } else if (strcmp("duration", feild) == 0)   {
                ptr->duration = value;
            } else if (strcmp("name", feild) == 0)       {
                ptr->name = value;
            } else if (strcmp("description", feild) == 0){
                ptr->description = value;
            } else if (strcmp("location", feild) == 0)   {
                ptr->location = value;
            } else {
                printf("Issue with feildname %s\n", feild);
            }
        }
        ptr = ptr->next;
    }
}

int close_request(request *req){
    free(req);
}

int save_request(request *req, Calendar *cal)
{
    // NOTE! OPENING ON EACH OP AFFECTS PREFORMACE
    // If we are too slow, we need to fix this
    // Adds request string to the file
    FILE *fp;
    if (cal->fp){
        fp = cal->fp;
    } else {
        printf("Reopening file, something went wrong\n");
        fp = fopen("data/JoeC", "a");
    }
    fputs(req->OG, fp);
    fputs("\n", fp);
    fflush(fp);
    close_request(req);
    cal->fp = fp;
    return 0;
}
