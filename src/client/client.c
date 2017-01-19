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
#include <arpa/inet.h>

#include "client.h"
#include "crypt.h"
#include "message.h"
#include "window.h"
#include "notify.h"

extern int PORT;
static char sv_name[NAME_LEN] = {};
static char cl_name[NAME_LEN] = {};

static int quit_condition = 0;
static pthread_mutex_t msg_mutex;

aes_t key[KEY_SIZE], iv[KEY_SIZE];

static void debuglog(char *s)
{
    int fd = open("log", O_CREAT|O_WRONLY|O_APPEND);
    write(fd, s, strlen(s));
    close(fd);
}

static void exchange_keys(int sv_fd)
{
    send(sv_fd, key, sizeof(key), 0);
    send(sv_fd, iv, sizeof(iv), 0);
}

static void share_names(int cl_fd)
{
    struct packet packet;
    int length = encrypt(cl_name, key, iv, packet.data);
    packet.length = length;
    send(cl_fd, &packet, sizeof(packet), 0);

    recv(cl_fd, &packet, sizeof(packet), -1);
    length = decrypt(packet.data, packet.length, key, iv, sv_name);
}

static int sock_setup(char sv_nameaddr[64])
{
    int sockfd;
    struct sockaddr_in6 sv_addr;

    sockfd = socket(AF_INET6, SOCK_STREAM, 0);

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, sv_nameaddr, &(sv_addr.sin6_addr));
    sv_addr.sin6_port = htons(PORT);

    connect(sockfd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));

    exchange_keys(sockfd);
    share_names(sockfd);
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

    struct packet packet;
    while (1)
    {
        int nbytes = recv(sv_fd, &packet, sizeof(packet), 0);
        if (nbytes && nbytes != -1)
        {
            char decrypted[MESSAGE_BUFFER_SIZE] = {};
            int length = decrypt(packet.data, packet.length, key, iv, decrypted);
            if (strcmp(decrypted, "/quit") == 0)
            {
                pthread_mutex_lock(&msg_mutex);
                add_msg(d, "CChat", "The connection has been closed.");
                update_window(d);
                notify(d);
                pthread_mutex_unlock(&msg_mutex);
                quit_condition = 1;
                break;
            }

            pthread_mutex_lock(&msg_mutex);
            add_msg(d, sv_name, decrypted);
            update_window(d);
            notify(d);
            pthread_mutex_unlock(&msg_mutex);
        }
    }
    return 0;
}

void client(char sv_nameaddr[64], char nick[64])
{
    strncpy(cl_name, nick, NAME_LEN-1);

    generate_bytes(key);
    generate_bytes(iv);
    int sv_fd = sock_setup(sv_nameaddr);
    printf("Connection established.\n");

    msgs_data d;
    d.curr = 0;
    d.size = 100;
    d.msg_list = calloc(d.size, sizeof(msg));

    init_window();

    struct argl args;
    args.d = &d;
    args.sv_fd = sv_fd;
    pthread_t input_thread;//, output_thread;
    pthread_create(&input_thread, NULL, input, (void*)&args);

    struct packet packet;

    while (!quit_condition)
    {
        update_window(&d);
        char msg[256] = {};
        get_input(msg);
        if (msg && *msg)
        {
            msg[strcspn(msg, "\n")] = 0;
            int length = encrypt(msg, key, iv, packet.data);
            packet.length = length;
            send(sv_fd, &packet, sizeof(packet), 0);

            if (strcmp(msg, "/quit") == 0)
            {
                quit_condition = 1;
                break;
            }

            pthread_mutex_lock(&msg_mutex);
            add_msg(&d, cl_name, msg);
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
