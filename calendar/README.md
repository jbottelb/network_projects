## Calendar Server
Written By Josh Bottelberghe and Joseph Sweilem


Constraints:
Calendar names may not include:
: or " or ,

### Networking Design
Information exchange in JSON


### Information
The server is backed by a calendar library we wrote that handles all the actual
operations. It writes to disk, and uses append so it is mostly atomic in
operations.  

The saving works by saving the operations that change the saved state. Then, on
startup, all the operations that change the data are replayed before additional requests are done.


###

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
        "duration": "indefinite",
        "name": "Joe",
        "description": "being mean",
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
