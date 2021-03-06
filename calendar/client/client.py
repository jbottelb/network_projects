#!/usr/bin/env python3

'''
Client for Calendar
Python 3, Joe and Josh
'''
import socket, sys
import json

ADDR = ("localhost", 41069)

def main():
    # TODO: validate length
    l = len(sys.argv)
    if l < 3:
        print("WRONG", len(sys.argv))
        exit(1)

    connections = open("../.mycal")
    c_data = json.load(connections)

    ADDR = (c_data["servername"], int(c_data["port"]))

    if sys.argv[2] == "input":
        inputs = open(sys.argv[3])
        data = json.load(inputs)
        
        for command in data:
            response = send_request(command)
            print(response)
    else:
        data = build_data()
        print(data)

        response = send_request(data)
        print(response)

def send_request(req):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect(ADDR)
        sock.sendall(json.dumps(req).encode())
        print("Request sent")
        res = sock.recv(4096)
    return res

def build_data():
    request = {}
    request["CALENDAR"] = sys.argv[1]
    request["ACTION"]   = (sys.argv[2]).upper()
    args = {}

    if sys.argv[2] == "add":
        for i in range(3, len(sys.argv)-1, 2):
            args[sys.argv[i]] = sys.argv[i+1]
    elif sys.argv[2] == "remove":
        args["identifier"] = sys.argv[3]
    elif sys.argv[2] == "update":
        args["identifier"] = sys.argv[3]
        args[sys.argv[4]] = sys.argv[5]
    elif sys.argv[2] == "get":
        args["date"] = sys.argv[3]
    elif sys.argv[2] == "getall":
        args["start_date"] = sys.argv[3]
        args["end_date"] = sys.argv[4]
    else:
        print("Invalid Command: Try Again")
        exit(1)

    request["ARGUMENTS"] = args
    return request

if __name__ == "__main__":
    main()
