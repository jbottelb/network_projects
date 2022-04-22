#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "server.h"
#include "player.h"
#include "wordle.h"
#include "cJSON.h"

#define PORT "41069"
#define GAMEPORT "41420"
#define SIZE 1000
#define BACKLOG 10
#define MAXPLAYERS 100

void start_game(char *port){
    // start up socket

    // create game instance select word, create "Board"

    // while loop to listen for everyone to join, with select for messaging

    // once everyones here, start playing

    /*
    Game Loop
    for loop for all guesses,
        tell everyone we started the round, then another while loop with select
        on players giving guess, handle that with another function
        break if someone wins
    */

    // calculate restuls, report results

}

/*
    Finds a player from players in the game by name
*/
Player *find_player(Player **ps, char *name){
    return NULL;
}

// deals with a player sending a guess
void handle_guess(Player *p, cJSON *data, Wordle *w){
    cJSON *guess_J = cJSON_GetObjectItemCaseSensitive(data, "guess");
    char *guess = guess_J->valuestring;
    if (in_word_list(guess) != 0){
        send_GuessResponse(p, guess, "no");
        return;
    }
    char *res = make_guess(guess, w);
    p->res = res;
    p->rec_time = "now";
    if (is_correct(res) == 0){
        p->correct = "yes";
        p->winner = "yes";
    } else {
        p->correct = "no";
        p->winner = "no";
    }
    p->state = GUESSED;
    // TODO: Score player
    send_GuessResponse(p, guess, "yes");
}

int main(int argc, char *argv[])
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    char *port = (char *)calloc(BUFSIZ, 1);
    int mt = -1;

    port = PORT;

    if (argc == 2)
        { mt = 0; }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (!p)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");
    Player players[MAXPLAYERS];
    int player_count = 0;

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);

        // Receive join or joininstance request
        char *message = accept_request(new_fd);

        printf("%s\n", message);

        //char* Json = "{\"MessageType\": \"Join\", \"data\": {\"name\": \"joe\", \"server\": \"localhost\", \"port\": \"41069\"}}";

        cJSON *join_result = cJSON_Parse(message);
        printf("%s\n", cJSON_Print(join_result));

        cJSON *data = cJSON_GetObjectItemCaseSensitive(data, "Data");
        cJSON *name = cJSON_GetObjectItemCaseSensitive(data, "Name");
        Player *p = create_player(name->valuestring, new_fd, 0, 0);

        /*
        message = "yes";
        int numread;
        int count = 0, it = 0;

        while (message[it++] != '\0') {
            count++;
        }

        if ((numread = send(new_fd, message, count, 0)) == -1) {
                printf("server: error sending data packet. \n");
                printf("%s\n", strerror(errno));
                exit(1);
        }
        */

        if (player_count == PLAYERS){
            signal(SIGCHLD, SIG_IGN);
            /* Fork off child process to handle request */
            pid_t pid = fork();

            if(pid < 0){
                continue;
            }
            else if(pid == 0) {
                start_game(GAMEPORT);
                /*
                    Message all players to join game lobby, then drop them
                */

                for (int i = 0; i < sizeof(players); i++) {
                    message_player(strcat("join port: ", GAMEPORT), &players[i]);
                }

                exit(0);
            }
        }

        // Closes socket and returns to listening for new connections
        close(new_fd);
    }

    return 0;
}

char *accept_request(int sock)
{
    int file_size, numbytes;
    char buf[SIZE];

    // Receives request from client
    if ((numbytes = recv(sock, buf, SIZE-1, 0)) == -1){
        printf("server: request was not received. \n");
        exit(1);
    }

    // Add null terminator
    buf[numbytes] = '\0';

    char* buff_pointer = (char *)calloc(SIZE, sizeof(char));
    strcpy(buff_pointer, buf);

    return buff_pointer;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}
