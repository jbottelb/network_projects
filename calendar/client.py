#!/usr/bin/env python3

'''
Client for Calendar
Python 3, Joe and Josh
'''
import socket, sys, time
import json

ADDR = ("localhost", 41069)

def main():
    # TODO: validate length
    l = len(sys.argv)
    if l < 3 or (l % 2 == 0 and l > 5):
        print("WRONG", len(sys.argv))
        exit(1)

    data = build_data()
    print(data)

    response = send_request(data)

    print(response)

def send_request(req):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect(ADDR)
        sock.sendall(json.dumps(req).encode())
        print("Request sent")
        res = sock.recv(1024)
    return res

def build_data():
    request = {}
    request["CALENDAR"] = sys.argv[1]
    request["ACTION"]   = sys.argv[2]
    args = {}
    for i in range(3, len(sys.argv)-1, 2):
        args[sys.argv[i]] = sys.argv[i+1]
    request["ARGUMENTS"] = args
    return request

if __name__ == "__main__":
    main()
