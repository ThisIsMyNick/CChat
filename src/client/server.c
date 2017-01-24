#include <errno.h>
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
#include "notify.h"
#include "server.h"
#include "window.h"

extern int PORT;
static char sv_name[NAME_LEN] = {};
static char cl_name[NAME_LEN] = {};

static int quit_condition = 0;
static pthread_mutex_t msg_mutex;

aes_t key[KEY_SIZE];
aes_t iv[KEY_SIZE];

// Passed into the worker function in the listening thread
struct argl
{
    msgs_data *d;
    int cl_fd;
};

/*
 * Reads the AES key and initialization vector from the client
 * to set up end-to-end encryption
 */
static void exchange_keys(int cl_fd)
{
    recv(cl_fd, key, sizeof(key), 0);
    recv(cl_fd, iv, sizeof(iv), 0);
}

/*
 * Share server name with the client, and receive the client's name
 */
static void share_names(int cl_fd)
{
    struct packet packet;
    recv(cl_fd, &packet, sizeof(packet), 0);
    int length = decrypt(packet.data, packet.length, key, iv, cl_name);
    if (length == -1)
    {
        fprintf(stderr, "Failed to decrypt client name.\n");
        exit(1);
    }

    length = encrypt(sv_name, key, iv, packet.data);
    packet.length = length;
    send(cl_fd, &packet, sizeof(packet), 0);
}


/*
 * Worker function that constantly listens for a message from a client
 * Meant to be run in a thread
 */
static void *input(void *args)
{
    struct argl *arg = (struct argl*)args;
    msgs_data *d = arg->d;
    int cl_fd = arg->cl_fd;

    struct packet packet;
    while (1)
    {
        int nbytes = recv(cl_fd, &packet, sizeof(packet), 0);
        if (nbytes && nbytes != -1)
        {
            char decrypted[MESSAGE_BUFFER_SIZE] = {};
            int length = decrypt(packet.data, packet.length, key, iv, decrypted);
            if (length == -1)
            {
                pthread_mutex_lock(&msg_mutex);
                add_msg(d, "CChat", "Decryption failed.");
                pthread_mutex_unlock(&msg_mutex);
                continue;
            }
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
            add_msg(d, cl_name, decrypted);
            update_window(d);
            notify(d);
            pthread_mutex_unlock(&msg_mutex);
        }
    }
    return 0;
}

/*
 * Handles the server loop for a specific client
 *
 * Params:
 * int cl_fd: File descriptor of the client to be served
 */
void serve(int cl_fd)
{
    msgs_data d;
    d.curr = 0;
    d.size = 10;
    d.msg_list = calloc(d.size, sizeof(msg));
    init_window();

    struct argl args;
    args.d = &d;
    args.cl_fd = cl_fd;
    pthread_t input_thread;
    pthread_create(&input_thread, NULL, input, (void*)&args);
    struct packet packet;

    while (!quit_condition)
    {
        update_window(&d);
        char msg[256] = {};
        get_input(msg);
        if (*msg)
        {
            int length = encrypt(msg, key, iv, packet.data);
            packet.length = length;
            send(cl_fd, &packet, sizeof(packet), 0);

            if (strcmp(msg, "/quit") == 0)
            {
                quit_condition = 1;
                break;
            }

            pthread_mutex_lock(&msg_mutex);
            add_msg(&d, sv_name, msg);
            update_window(&d);
            pthread_mutex_unlock(&msg_mutex);
        }
    }
    pthread_kill(input_thread, SIGTERM);
    close(cl_fd);
    free_msgs(&d);
    close_window();
    printf("Connection closed.\n");
}

/*
 * Runs the server loop
 *
 * Params:
 * char nick[64]: Server nickname
 */
void server(char nick[64])
{
    strncpy(sv_name, nick, NAME_LEN-1);

    int sockfd;
    struct sockaddr_in sv_addr, cl_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        fprintf(stderr, "Failed to set up socket: %s\n", strerror(errno));
        exit(1);
    }

    bzero(&sv_addr, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_addr.s_addr = INADDR_ANY;
    sv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr*)&sv_addr, sizeof(sv_addr)) == -1)
    {
        fprintf(stderr, "Failed to bind port: %s\n", strerror(errno));
        exit(1);
    }

    if (listen(sockfd, 5) == -1)
    {
        fprintf(stderr, "Failed to listen: %s\n", strerror(errno));
        exit(1);
    }

    while (1)
    {
        // Only allow one client to be connected at a time
        socklen_t cl_len;
        int cl_fd = accept(sockfd, (struct sockaddr*)&cl_addr, &cl_len);
        printf("Connection established.\n");
        exchange_keys(cl_fd);
        share_names(cl_fd);
        serve(cl_fd);
    }
}
