#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "client.h"

extern int PORT;

static void handshake(int sv_fd)
{
    char buf[10] = "sup";
    send(sv_fd, buf, sizeof(buf), 0);

    char buf2[10] = {};
    recv(sv_fd, buf2, sizeof(buf2), 0);

    if (strcmp(buf2, "im here") != 0)
    {
        fprintf(stderr, "Unable to set up connection.\n");
        exit(1);
    }

    char buf3[10] = "all good";
    send(sv_fd, buf3, sizeof(buf3), 0);
}

static int sock_setup(char sv_name[64])
{
    int sockfd;
    struct sockaddr_in sv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server = gethostbyname(sv_name);

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    memcpy(server->h_addr, &sv_addr.sin_addr.s_addr, server->h_length);
    sv_addr.sin_port = htons(PORT);

    connect(sockfd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));

    handshake(sockfd);
    return sockfd;
}

void client(char sv_name[64], char nick[64])
{
    int sv_fd = sock_setup(sv_name);
    printf("Connection established.\n");

    while (1)
    {
        printf("Input: ");
        char msg[512] = {};
        fgets(msg, sizeof(msg), stdin);
        msg[strcspn(msg, "\n")] = 0;
        send(sv_fd, msg, sizeof(msg), 0);

        if (strcmp(msg, "/quit") == 0)
            break;

        char response[512] = {};
        recv(sv_fd, response, sizeof(response), 0);
        printf("[*] Received: %s\n", response);

        if (strcmp(response, "/quit") == 0)
            break;
    }
    close(sv_fd);
    printf("Connection closed.\n");
    exit(0);
}
