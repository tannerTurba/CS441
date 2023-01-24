#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

/* This code was created by referencing Beej's Guide to IPC. */
int main(int argc, char *argv[])
{
    int s, t, len;
    struct sockaddr_un remote;
    char str[100];
    char * SOCK_PATH = argv[1];

    /* Create a Unix domain socket for communication */
    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    /* 
     * Set up the sockaddr_un struct called "remote" and use connect
     * to setting up communication through socket
     */
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(s, (struct sockaddr *)&remote, len) == -1) {
        perror("connect");
        exit(1);
    }

    /* Use recv to retrieve messages */
    if ((t=recv(s, str, 100, 0)) > 0) {
        str[t] = '\0';
        printf("%s", str);
    } else {
        if (t < 0) perror("recv");
        else printf("Server closed connection\n");
        exit(1);
    }

    /* Safely close socket and return with status code 0 */
    close(s);
    return 0;
}