## Calendar Server


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
        date: "Tuesday",
        time: "now",
        duration: "indefinite",
        name: "Joe",
        description: "being mean",
        location: "everywhere"
    }
}
