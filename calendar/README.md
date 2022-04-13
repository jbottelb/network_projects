## Calendar Server
Written By Josh Bottelberghe and Joseph Sweilem \
Total time: 50 hours

We were granted a TWO DAY extension on the project

## Overview
To show the code works for all the commands, run
`make clean`,
`make`,
on the server `./server`
for the client:
`./client.py JoeC input inputs.json`

This will display the server handling a number of inputs.

The code is broken into a calendar class, which handles the calendar operations,
and a server file, which runs the server and uses that class to handle the requests.

## Persistance
Persistance is handled with a log file. Commands that change the calendar
are logged to a file corresponding to that calendar name. It is done through appends,
so if the server crashes in between requests, the file will still hold the changes,
so the server can crash. These files are read back when the calendar is accessed
to get the accurate state of the calendar.


### JSON
We wrote the bulk of this code before the helpful JSON libraries were posted on the due
date. Therefore, we wrote the server to parse the requests on its own. This limits
flexibilty.

## Client
The client runs as requested. We did it in Python so there is no Makefile necessary for the executable.

## Server
The server functionality is best explored by the file test.c, this gives
a good understanding of what to expect from the calendar library. 
The makefile to compile the server pieces is in the server directory.

### Examples

Attributes:
Calendar
Action
Arguments: {fields}

EX:
{
    "CALENDAR": "Joe's Calendar",
    "Action": ADD,
    "Arguments": {
        "date": 012222,
        "time": 1233,
        "name": "Joe",
        "duration": "indefinite",
        "description": "being cool",
        "location": "everywhere"
    }
}

{
    "CALENDAR": "Joe's Calendar",
    "Action": REMOVE,
    "Arguments": {
        "identifier": 0
    }
}
