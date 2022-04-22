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
#include "player.h"

/*

*/
void message_player(char *message, Player p)
{
    int numread;
    int count = 0, it = 0;

    while (message[it++] != '\0') {
        count++;
    }

    if ((numread = send(p.socket, message, count, 0)) == -1) {
            printf("server: error sending data packet. \n");
            exit(1);
    }
}
