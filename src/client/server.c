#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "message.h"
#include "server.h"
#include "window.h"

extern int PORT;

void serve(int cl_fd)
{
    int msgs_size = 10;
    int msgs_curr = 0;
    msg* msg_list = calloc(msgs_size, sizeof(msg));
    init_window();
    while (1)
    {
        update_window(msg_list, msgs_curr);
        char buf[256] = {};
        recv(cl_fd, buf, sizeof(buf), 0);
        add_msg(msg_list, &msgs_size, &msgs_curr, "Client", buf);
        update_window(msg_list, msgs_curr);

        if (strcmp(buf, "/quit") == 0)
            break;

        char msg[256] = {};
        get_input(msg);
        if (msg != 0)
        {
            send(cl_fd, msg, sizeof(msg), 0);
            add_msg(msg_list, &msgs_size, &msgs_curr, "Server", msg);
            update_window(msg_list, msgs_curr);

            if (strcmp(msg, "/quit") == 0)
                break;
        }

    }
    close(cl_fd);
    close_window();
    printf("Connection closed.\n");
}


static void handshake(int cl_fd)
{
    char buf[10] = {};
    recv(cl_fd, buf, sizeof(buf), 0);

    if (strcmp(buf, "sup") != 0)
    {
        fprintf(stderr, "Unable to set up connection.\n");
        exit(1);
    }

    char buf2[10] = "im here";
    send(cl_fd, buf2, sizeof(buf2), 0);

    char buf3[10] = {};
    recv(cl_fd, buf3, sizeof(buf3), 0);

    if (strcmp(buf3, "all good") != 0)
    {
        fprintf(stderr, "Unable to set up connection.\n");
        exit(1);
    }
}

void server()
{
    int sockfd;
    struct sockaddr_in sv_addr, cl_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&sv_addr, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_addr.s_addr = INADDR_ANY;
    sv_addr.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    listen(sockfd, 5);

    while (1)
    {
        socklen_t cl_len;
        int cl_fd = accept(sockfd, (struct sockaddr*)&cl_addr, &cl_len);
        if (fork() == 0)
        {
            handshake(cl_fd);
            printf("Connection established.\n");

            serve(cl_fd);
            exit(0);
        }
        close(cl_fd);
    }
}
