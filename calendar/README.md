## Calendar Server


Constraints:
Calendar names may not include:
: or " or ,

### Networking Design
Information exchange in JSON

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
