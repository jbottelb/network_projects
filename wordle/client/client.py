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

def hacky_recv(sock):
    rec = b''
    while True:
        rec += sock.recv(128)
        try:
            return json.loads(rec.decode())
        except:
            continue

def send_request(req):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        if req["MessageType"] == "Join":
            sock.connect((req["Data"]["Server"], int(req["Data"]["Port"])))
            sock.sendall(json.dumps(req).encode())

            join = hacky_recv(sock)
            print(join)

            if join["Data"]["Result"] == "no":
                exit(0)
            else:
                join = hacky_recv(sock)
                print(join)
        elif req["MessageType"] == "JoinInstance":
            sock.connect((req["Data"]["Server"], int(req["Data"]["Port"])))
            sock.sendall(json.dumps(req).encode())

            join = hacky_recv(sock)
            print(join)

            if join["Data"]["Result"] == "no":
                exit(0)
            else:
                # Start Game message
                result = hacky_recv(sock)
                print(result)

                for _ in range(int(result["Data"]["Rounds"])):
                    # Start Round
                    result = hacky_recv(sock)
                    print(result)

                    # Prompt for Guess
                    result = hacky_recv(sock)
                    print(result)

                    # Guess logic
                    while True:
                        # Build and send Guess
                        guess, args = {}, {}
                        for line in sys.stdin:
                            line = line.split()
                            guess["MessageType"] = line[0]
                            args["Name"] = line[1]
                            args["Guess"] = line[2]
                            guess["Data"] = args
                            print(guess)
                            sock.sendall(json.dumps(guess).encode())

                            # Guess Response
                            result = hacky_recv(sock)
                            print(result)

                            if result["Data"]["Accepted"] == "yes":
                                break
                            else:
                                # Prompt for Guess
                                result = hacky_recv(sock)
                                print(result)
                        break
                    print("guess valid")

                    # Guess Result
                    result = hacky_recv(sock)
                    print(result)

                    # End Round
                    result = hacky_recv(sock)
                    print(result)

                    win = False
                    for p in result["Data"]["PlayerInfo"]:
                        if p["Winner"] == "yes":
                            print("Game won by", p["Name"])
                            win = True

                    if win:
                        break
                # End Game
                result = hacky_recv(sock)
                print(result)
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
