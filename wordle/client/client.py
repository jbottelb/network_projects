#!/usr/bin/env python3

'''
Client for Calendar
Python 3, Joe and Josh
'''
import socket, sys
import json
import threading

NAME = "Not Set"

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
    while True:
        r = {}
        rec = b''
        while True:
            rec += sock.recv(128)
            try:
                r = json.loads(rec.decode())
                break
            except:
                continue
        if r["MessageType"] == "Chat":
            print(r["Data"]["Name"] + ": " + r["Data"]["Text"])
        elif r["MessageType"] == "PromptForGuess":
            print("mpwordle: The server prompts for a guess")
        elif r["MessageType"] == "StartRound":
            print("mpwordle: The Round has started")
        elif r["MessageType"] == "StartGame":
            print("mpwordle: The Game has started")
        elif r["MessageType"] == "JoinResult":
            if r["Data"]["Result"] == "yes":
                print("mpwordle: Player Entered Waiting Lobby")
                print("mpwordle: Waiting for server, feel free to chat")
            else:
                print("Failed to enter Lobby")
        elif r["MessageType"] == "JoinInstanceResult":
            if r["Data"]["Result"] == "yes":
                print("mpwordle: Player Entered Game Lobby")
            else:
                print("Failed to enter Game Lobby")
        elif r["MessageType"] == "GuessResponse":
            if r["Data"]["Accepted"] == "yes":
                print("mpwordle: Guess accepted")
            else:
                print("mpwordle: Word not in list. Guess again")
        elif r["MessageType"] == "StartInstance":
            print("Nonce: " + str(r["Data"]["Nonce"]), "Port:", r["Data"]["Port"])
            print("mpwordle: Type exit or hit enter/return to quit the waiting lobby and join the game!")
            exit(0)
        elif r["MessageType"] == "EndRound":
            print("mpwordle: Round recap")
            for player in r["Data"]["PlayerInfo"]:
                print(player["Name"] + ": " + str(player["Score"]))
        elif r["MessageType"] == "EndGame":
            print("mpwordle: The game is over!")
            print("Winner: " + r["Data"]["WinnerName"])
            exit(0)
        elif r["MessageType"] == "GuessResult":
            dat = r["Data"]["PlayerInfo"]
            for e in dat:
                if e["Name"] == NAME:
                    print("mpwordle: Reponse: " + e["Result"])
        else:
            # anything we havent coded
            pass


def chat_handler(sock):
    m = {}
    args = {}
    for line in sys.stdin:
        if "exit" in line:
            exit(0)
        m["MessageType"] = "Chat"
        args["Name"] = NAME
        args["Text"] = line.strip()
        m["Data"] = args
        print(m)
        sock.sendall(json.dumps(m).encode())

def send_request(req):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        # spawn a listener, which accepts and interprets all responses
        sock.connect((req["Data"]["Server"], int(req["Data"]["Port"])))
        sock.sendall(json.dumps(req).encode())
        listener = threading.Thread(target=hacky_recv, args=(sock,))
        listener.start()
        if req["MessageType"] == "Join":
            x = threading.Thread(target=chat_handler, args=(sock,))
            x.start()
            x.join()
            sys.exit(0)
        elif req["MessageType"] == "JoinInstance":
            while True:
                print("mpwordle: Enter Chat or Guess <guess string>")
                for line in sys.stdin:
                    try:
                        guess, args = {}, {}
                        line = line.split()
                        guess["MessageType"] = line[0]
                        if (guess["MessageType"] == "Chat"):
                            print("mpwordle: Type your message")
                            m = {}
                            message = input()
                            m["MessageType"] = "Chat"
                            args["Name"] = NAME
                            args["Text"] = message.strip()
                            m["Data"] = args
                            sock.sendall(json.dumps(m).encode())
                            flag = "again"
                        else:
                            args["Name"] = NAME
                            args["Guess"] = line[1]
                            guess["Data"] = args
                            sock.sendall(json.dumps(guess).encode())
                    except:
                        print("mpwordle: Issue with input, try again")
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
    NAME = sys.argv[3]
    main()
