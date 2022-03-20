## Calendar Server


Constraints:
Calendar names may not include:
: or " or ,

### Networking Design
Information exchange in JSON


### Information
The server is backed by a calendar library I wrote that handles all the actual
operations. It writes to disk, and uses append so it is mostly atomic in
operations.  


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
