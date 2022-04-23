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
    if l == 8 or l == 10:
        data = build_data_join()
        send_request(data)

    else:
        print("WRONG", len(sys.argv))
        exit(1)

def send_request(req):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        if req["MessageType"] == "Join":
            sock.connect((req["Data"]["Server"], int(req["Data"]["Port"])))
            sock.sendall(json.dumps(req).encode())

            rec = sock.recv(4096)
            join = json.loads(rec.decode())
            print(join)

            if join["Data"]["Result"] == "no":
                exit(0)
            else:
                rec = sock.recv(4096)
                join = json.loads(rec.decode())
                print(join)
        elif req["MessageType"] == "JoinInstance":
            sock.connect((req["Data"]["Server"], int(req["Data"]["Port"])))
            sock.sendall(json.dumps(req).encode())

            rec = sock.recv(4096)
            join = json.loads(rec.decode())
            print(join)

            if join["Data"]["Result"] == "no":
                exit(0)
            else:
                rec = sock.recv(4096)
                join = json.loads(rec.decode())
                print(join)

                '''
                while result["MessageType"] != "EndGame":
                    print("made it here")
                    #result = sock.recv(4096)
                    #print(result)
                    #Build request listen types and responses
                    exit(0)
                '''
        else:
            print("Something went wrong if we got here")
            exit(1)




def build_data_join():
    request = {}
    args = {}

    request["MessageType"] = sys.argv[1]

    args["Name"] = sys.argv[3]
    args["Server"] = sys.argv[5]
    args["Port"] = sys.argv[7]

    if sys.argv[1] == "JoinInstance":
        args["Nonce"] = int(sys.argv[9])
    elif sys.argv[1] == "Join":
        pass
    else:
        print("Invalid Command: Try Again")
        exit(1)

    request["Data"] = args

    return request

if __name__ == "__main__":
    main()
