'''
Client for Calendar
Python 3, Joe and Josh
'''
import socket, sys, time

ADDR = ("localhost", 9000)

def main():
    # TODO: validate length
    l = len(sys.argv)
    if l < 3 or (l % 2 == 0 and l > 5):
        print("WRONG", len(sys.argv))
        exit(1)

    request_json = handle_args()
    print(request_json)

    response = send_request(request_json)

    print(response)

def send_request(req):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect(ADDR)
        sock.sendall(bytes(req))
        res = sock.recv(1024)
    return res

def handle_args():
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
