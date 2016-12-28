#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "client.h"
#include "crypt.h"
#include "message.h"
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

static void handshake(int sv_fd)
{
    send(sv_fd, key, sizeof(key), 0);
    send(sv_fd, iv, sizeof(iv), 0);

    aes_t buf[MESSAGE_BUFFER_SIZE];
    recv(sv_fd, buf, MESSAGE_BUFFER_SIZE, 0);
    if (strncmp(decrypt(buf, key, iv), "im here", MESSAGE_BUFFER_SIZE) != 0) {
        fprintf(stderr, "Unable to set up connection.\n");
        exit(1);
    }

    char *buf2 = "all good";
    send(sv_fd, encrypt(buf2, key, iv), MESSAGE_BUFFER_SIZE, 0);
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
        aes_t response[MESSAGE_BUFFER_SIZE] = {};
        int nbytes = recv(sv_fd, response, sizeof(response), 0);
        if (nbytes && nbytes != -1)
        {
            char *decrypted = decrypt(response, key, iv);
            if (strcmp(decrypted, "/quit") == 0)
            {
                quit_condition = 1;
                break;
            }

            pthread_mutex_lock(&msg_mutex);
            add_msg(d, "Server", decrypted);
            update_window(d);
            pthread_mutex_unlock(&msg_mutex);
        }
    }
    return 0;
}

void client(char sv_name[64], char nick[64])
{
    generate_key(key);
    generate_iv(iv);
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
    pthread_t input_thread;//, output_thread;
    pthread_create(&input_thread, NULL, input, (void*)&args);

    while (!quit_condition)
    {
        update_window(&d);
        char msg[256] = {};
        get_input(msg);
        if (msg && *msg)
        {
            msg[strcspn(msg, "\n")] = 0;
            aes_t *encrypted = encrypt(msg, key, iv);
            send(sv_fd, encrypted, MESSAGE_BUFFER_SIZE, 0);

            if (strcmp(msg, "/quit") == 0)
            {
                quit_condition = 1;
                break;
            }

            pthread_mutex_lock(&msg_mutex);
            add_msg(&d, "Client", msg);
            update_window(&d);
            pthread_mutex_unlock(&msg_mutex);
        }
    }
    pthread_kill(input_thread, SIGTERM);
    close(sv_fd);
    free_msgs(&d);
    close_window();
    printf("Connection closed.\n");
    exit(0);
}
