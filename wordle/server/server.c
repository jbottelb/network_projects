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
#include <sys/select.h>
#include "server.h"
#include "player.h"
#include "wordle.h"
#include "cJSON.h"

#define PORT "41069"
#define GAMEPORT "41420"
#define SIZE 1000
#define BACKLOG 10
#define MAXPLAYERS 100
#define ROUNDS 6
#define SLEEPTIME 2

void start_game(char *new_port, Player **players, int nonce){
    printf("GAME LAUNCHED\n");
    printf("GAME NONCE: %d\n", nonce);

    fd_set master;
    fd_set read_fds;
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    int fdmax;

    // start up socket
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    char *port = (char *)calloc(BUFSIZ, 1);
    port = new_port;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
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
        fprintf(stderr, "game: failed to bind\n");
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

    printf("game: waiting for connections...\n");

    FD_SET(sockfd, &master);
    fdmax = sockfd;

    int player_count = 0;
    int start_game = 1;

    while(start_game == 1) {
        printf("Waiting for a player to connect\n");
        read_fds = master;
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
            printf("Failed to select\n");
            exit(SLEEPTIME);
        }
        for(int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == sockfd) {
                    printf("Listener found connection\n");
                    sin_size = sizeof their_addr;
                    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
                    if (new_fd == -1) {
                        perror("accept");
                        exit(1);
                    }
                    FD_SET(new_fd, &master);
                    if (new_fd > fdmax) {
                        fdmax = new_fd;
                    }

                    inet_ntop(their_addr.ss_family,
                        get_in_addr((struct sockaddr *)&their_addr),
                        s, sizeof s);
                    printf("game: got connection from %s\n", s);

                    // Receive join or joininstance request
                    char *message = accept_request(new_fd);
                    printf("%s\n", message);

                    cJSON *join_result = cJSON_Parse(message);
                    Player *p = recv_JoinInstance(join_result, new_fd, player_count);
                    players[player_count] = p;

                    char* response = "yes";
                    send_JoinInstanceResult(response, p); // check rejection
                    sleep(SLEEPTIME);
                    player_count++;
                }
            }
        }

        if (player_count == PLAYERS){
            for (int i = 0; i < player_count; i++) {
                send_StartGame(ROUNDS, players, players[i]);
                printf("sent start game\n");
                sleep(SLEEPTIME);
                start_game = 0;
            }
        }
    }


    // create game instance select word, create "Board"
    Wordle *w = create_board("Fucknuts", ROUNDS);
    char *word = (char *) calloc(BUFSIZ, sizeof(char));
    word = select_word(w);

    printf("Word is %s\n", word);

    int round = 1;
    int game_over = 0;

    while (round <= ROUNDS && game_over == 0){
        // Start
        for (int i = 0; i < player_count; i++) {
            send_StartRound(w->wordlen, round, ROUNDS - round, players, players[i]);
            printf("sent start round\n");
            sleep(SLEEPTIME);
            send_PromptForGuess(w->wordlen, players[i], round);
            printf("sent prompt for guess\n");
        }

        // assuming players behave
        int num_guesses = 0;
        // Select implementation... now
        while(num_guesses < PLAYERS) {
            read_fds = master;
            if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
                printf("Failed to select\n");
                exit(4);
            }
            for(int i = 0; i <= fdmax; i++) {
                if (FD_ISSET(i, &read_fds)) {
                    sleep(SLEEPTIME);
                    char* guess_json = (char *)calloc(SIZE, sizeof(char));
                    guess_json = accept_request(i);
                    printf("%s\n", guess_json);

                    cJSON *guess_result = cJSON_Parse(guess_json);
                    cJSON *type = cJSON_GetObjectItemCaseSensitive(guess_result, "MessageType");
                    if (strcmp(type->valuestring, "Guess") == 0) {
                        cJSON *data = cJSON_GetObjectItemCaseSensitive(guess_result, "Data");
                        cJSON *name = cJSON_GetObjectItemCaseSensitive(data, "Name");

                        Player *p = find_player(players, name->valuestring);
                        printf("Player found: %s\n", p->name);
                        printf("Expected player: %s\n", name->valuestring);

                        char* guess = recv_Guess(guess_result, p);
                        printf("%s\n", guess);

                        if (in_word_list(guess) == 0) {
                            send_GuessResponse(p, guess, "yes");
                        }
                        else {
                            send_GuessResponse(p, guess, "no");
                            break;
                        }
                        printf("sent guess response\n");

                        sleep(SLEEPTIME);
                        char *res = make_guess(guess, w);
                        p->score += score_guess(res, round);
                        p->res = res;

                        num_guesses++;

                        if (is_correct(res) == 0){
                            // correct guess
                            char *yes = (char *)calloc(5 /* Just in case */, sizeof(char));
                            p->winner  = "yes";
                            p->correct = "yes";
                            printf("A player has guessed correctly.\n");
                            game_over = 1;
                            send_GuessResult(p, players, "yes");
                        } else {
                            send_GuessResult(p, players, "no");
                        }
                    } else {
                        printf("Accepted chat\n");
                        sleep(SLEEPTIME);

                        cJSON *data   = cJSON_GetObjectItemCaseSensitive(guess_result, "Data");
                        cJSON *j_name = cJSON_GetObjectItemCaseSensitive(data, "Name");
                        cJSON *j_text = cJSON_GetObjectItemCaseSensitive(data, "Text");
                        for (int k = 0; k < PLAYERS; k++){
                            if (players[k]->socket == i){
                                continue;
                            }
                            printf("Relaying chat %s: %s\n", j_name->valuestring, censor(j_text->valuestring));
                            send_Chat(censor(j_text->valuestring), j_name->valuestring, players[k]);
                        }
                    }
                }
            }
        }
        w->count++;
        sleep(SLEEPTIME);
        for (int i = 0; i < player_count; i++) {
            send_EndRound(players[i], players, ROUNDS - round);
            printf("sent end round\n");
        }
        sleep(SLEEPTIME);
        round++;
    }

    // choose winner by score, pass to endgame

    // end
    for (int i = 0; i < player_count; i++) {
        send_EndGame(players[i], players[i]->name, players);
        printf("sent end game\n");
    }
}

// deals with a player sending a guess
void handle_guess(Player *p, cJSON *data, Wordle *w){
    cJSON *guess_J = cJSON_GetObjectItemCaseSensitive(data, "guess");
    char *guess = guess_J->valuestring;
    if (in_word_list(guess) != 0){
        send_GuessResponse(p, guess, "no");
        sleep(1);
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
    // socket things
    fd_set master;
    fd_set read_fds;
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    int fdmax;

    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    char *port = (char *)calloc(BUFSIZ, 1);
    port = PORT;

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
    Player **players = (Player **)calloc(MAXPLAYERS, sizeof(Player *));
    int nonce = rand() % BUFSIZ;
    int player_count = 0;

    FD_SET(sockfd, &master);
    fdmax = sockfd;

    while(1) {  // main accept() loop
        read_fds = master;
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
            exit(4);
        }

        for (int i = 0; i <= fdmax; i++)
        {
            if (FD_ISSET(i, &read_fds)){
                if (i == sockfd){
                    sin_size = sizeof their_addr;
                    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
                    if (new_fd == -1) {
                        perror("accept");
                        continue;
                    }

                    FD_SET(new_fd, &master);
                    if (new_fd > fdmax){
                        fdmax = new_fd;
                    }

                    inet_ntop(their_addr.ss_family,
                        get_in_addr((struct sockaddr *)&their_addr),
                        s, sizeof s);
                    printf("server: got connection from %s\n", s);

                    // Receive join or joininstance request
                    char *message = accept_request(new_fd);

                    printf("%s\n", message);

                    cJSON *join_result = cJSON_Parse(message);

                    cJSON *data = cJSON_GetObjectItemCaseSensitive(join_result, "Data");
                    cJSON *name = cJSON_GetObjectItemCaseSensitive(data, "Name");

                    Player *p = create_player(name->valuestring, new_fd, player_count, nonce);
                    players[player_count] = p;
                    player_count++;

                    char* response = "yes";
                    send_JoinResult(response, p);
                } else {
                    sleep(SLEEPTIME);
                    char* guess_json = (char *)calloc(SIZE, sizeof(char));
                    guess_json = accept_request(i);
                    cJSON *chat_result = cJSON_Parse(guess_json);
                    cJSON *data   = cJSON_GetObjectItemCaseSensitive(chat_result, "Data");
                    cJSON *j_name = cJSON_GetObjectItemCaseSensitive(data, "Name");
                    cJSON *j_text = cJSON_GetObjectItemCaseSensitive(data, "Text");
                    for (int k = 0; k < player_count; k++){
                        if (players[k]->socket == i){
                            continue;
                        }
                        printf("Relaying chat %s: %s\n", j_name->valuestring, censor(j_text->valuestring));
                        send_Chat(censor(j_text->valuestring), j_name->valuestring, players[k]);
                    }
                }
            }
        }

        if (player_count == PLAYERS){
            signal(SIGCHLD, SIG_IGN);
            /* Fork off child process to handle request */
            pid_t pid = fork();

            if(pid < 0){
                continue;
            }
            else if(pid == 0) {
                //Message all players to join game lobby, then drop them
                for (int i = 0; i < player_count; i++) {
                    send_StartInstance(players[i], "localhost", GAMEPORT);

                    // Closes socket and returns to listening for new connections
                    close(players[i]->socket);
                }

                start_game(GAMEPORT, players, nonce);
            }
            player_count = 0;
        }
    }

    return 0;
}

char *accept_request(int sock)
{
    int file_size, numbytes;
    char buf[SIZE];

    // Receives request from client
    if ((numbytes = recv(sock, buf, SIZE-1, 0)) == -1){
        printf("accept request: request was not received. \n");
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
