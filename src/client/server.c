#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "crypt.h"
#include "message.h"
#include "server.h"
#include "window.h"

extern int PORT;

static int quit_condition = 0;
static pthread_mutex_t msg_mutex;

aes_t key[KEY_SIZE], iv[KEY_SIZE];

static void debuglog(char *s)
{
    int fd = open("log", O_CREAT|O_WRONLY|O_APPEND);
    write(fd, s, strlen(s));
    close(fd);
}

struct argl
{
    msgs_data *d;
    int cl_fd;
};

static void *input(void *args)
{
    struct argl *arg = (struct argl*)args;
    msgs_data *d = arg->d;
    int cl_fd = arg->cl_fd;
    while (1)
    {
        aes_t response[MESSAGE_BUFFER_SIZE] = {};
        int nbytes = recv(cl_fd, response, sizeof(response), 0);
        if (nbytes && nbytes != -1)
        {
            char *decrypted = decrypt(response, key, iv);
            if (strcmp(decrypted, "/quit") == 0)
            {
                quit_condition = 1;
                break;
            }

            pthread_mutex_lock(&msg_mutex);
            add_msg(d, "Client", decrypted);
            update_window(d);
            pthread_mutex_unlock(&msg_mutex);
        }
    }
}

void serve(int cl_fd)
{
    msgs_data d;
    d.curr = 0;
    d.size = 10;
    d.msg_list = calloc(d.size, sizeof(msg));
    init_window();
    //fcntl(cl_fd, F_SETFL, fcntl(cl_fd, F_GETFL) | O_NONBLOCK);

    struct argl args;
    args.d = &d;
    args.cl_fd = cl_fd;
    pthread_t input_thread;
    pthread_create(&input_thread, NULL, input, (void*)&args);

    while (!quit_condition)
    {
        update_window(&d);
        char msg[256] = {};
        get_input(msg);
        if (msg && *msg)
        {
            aes_t *encrypted = encrypt(msg, key, iv);
            send(cl_fd, encrypted, MESSAGE_BUFFER_SIZE, 0);
            if (strcmp(msg, "/quit") == 0) {
                quit_condition = 1;
                break;
            }

            pthread_mutex_lock(&msg_mutex);
            add_msg(&d, "Server", msg);
            update_window(&d);
            pthread_mutex_unlock(&msg_mutex);

        }

    }
    pthread_kill(input_thread, SIGTERM);
    //pthread_join(input_thread, NULL);
    close(cl_fd);
    free_msgs(&d);
    close_window();
    printf("Connection closed.\n");
}

static void handshake(int cl_fd)
{
    recv(cl_fd, key, sizeof(key), 0);
    recv(cl_fd, iv, sizeof(iv), 0);

    char *buf = "im here";
    send(cl_fd, encrypt(buf, key, iv), MESSAGE_BUFFER_SIZE, 0);

    aes_t buf2[MESSAGE_BUFFER_SIZE];
    recv(cl_fd, buf2, MESSAGE_BUFFER_SIZE, 0);
    if (strncmp(decrypt(buf2, key, iv), "all good", MESSAGE_BUFFER_SIZE) != 0) {
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

    while (!quit_condition)
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
    }
}
