#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include "client.h"
#include "message.h"
#include "window.h"

extern int PORT;

pthread_mutex_t msg_mutex;

static void debuglog(char *s)
{
    int fd = open("log", O_CREAT|O_WRONLY|O_APPEND);
    write(fd, s, strlen(s));
    close(fd);
}

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

struct argl
{
    msgs_data *d;
    int sv_fd;
};

static void *input(void *args)
{
    struct argl *arg = (struct argl*)args;
    msgs_data *d = arg->d;
    int sv_fd = arg->sv_fd;
    while (1)
    {
        char response[256] = {};
        int nbytes = recv(sv_fd, response, sizeof(response), 0);
        if (nbytes && nbytes != -1)
        {
            add_msg(d, "Server", response);
            update_window(d);

            if (strcmp(response, "/quit") == 0)
                break;
        }
    }
}

void client(char sv_name[64], char nick[64])
{
    int sv_fd = sock_setup(sv_name);
    printf("Connection established.\n");

    msgs_data d;
    d.curr = 0;
    d.size = 10;
    d.msg_list = calloc(d.size, sizeof(msg));

    init_window();
    //fcntl(sv_fd, F_SETFL, fcntl(sv_fd, F_GETFL)|O_NONBLOCK);

    struct argl args;
    args.d = &d;
    args.sv_fd = sv_fd;
    pthread_t input_thread;
    pthread_create(&input_thread, NULL, input, (void*)&args);
    while (1)
    {
        update_window(&d);
        char msg[256] = {};
        get_input(msg);
        if (msg && *msg)
        {
            msg[strcspn(msg, "\n")] = 0;
            send(sv_fd, msg, sizeof(msg), 0);
            add_msg(&d, "Client", msg);
            update_window(&d);

            if (strcmp(msg, "/quit") == 0)
                break;
        }

    }
    close(sv_fd);
    free_msgs(&d);
    close_window();
    printf("Connection closed.\n");
    exit(0);
}
