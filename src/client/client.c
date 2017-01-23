#include <arpa/inet.h>
#include <errno.h>
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
#include "notify.h"
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
    int sv_fd;
};

/*
 * Sends over the AES key and initialization vector to the server
 * to set up end-to-end encryption
 *
 * Params:
 * int sv_fd: File descriptor of the server to share the keys with
 */
static void exchange_keys(int sv_fd)
{
    send(sv_fd, key, sizeof(key), 0);
    send(sv_fd, iv, sizeof(iv), 0);
}

/*
 * Share client name with the server, and receive the server's name
 *
 * Params:
 * int cl_fd: File descriptor of the server to share the names with
 */
static void share_names(int cl_fd)
{
    struct packet packet;
    int length = encrypt(cl_name, key, iv, packet.data);
    packet.length = length;
    send(cl_fd, &packet, sizeof(packet), 0);

    recv(cl_fd, &packet, sizeof(packet), 0);
    length = decrypt(packet.data, packet.length, key, iv, sv_name);
    if (length == -1) {
        fprintf(stderr, "Failed to decrypt server name.\n");
        exit(1);
    }
}

/*
 * Connects to the server at the address sv_nameaddr and shares keys and names
 *
 * Params:
 * char sv_nameaddr[64]: The address of the server, represented as a string
 *
 * Returns the file descriptor for the server socket
 */
static int sock_setup(char sv_nameaddr[64])
{
    int sockfd;
    struct sockaddr_in sv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        fprintf(stderr, "Failed to set up socket: %s\n", strerror(errno));
        exit(1);
    }

    bzero(&sv_addr, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, sv_nameaddr, &(sv_addr.sin_addr)) == -1)
    {
        fprintf(stderr, "Bad host name: %s\n", strerror(errno));
        exit(1);
    }
    sv_addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr*)&sv_addr, sizeof(sv_addr)) == -1)
    {
        fprintf(stderr, "Could not connect to server.\n");
        exit(1);
    }

    exchange_keys(sockfd);
    share_names(sockfd);
    return sockfd;
}

/*
 * Worker function that constantly listens for a message from the server
 * Meant to be run in a thread
 *
 * Params:
 * void *args: Struct containing the arguments server file descriptor and the list of messages
 */
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
            add_msg(d, sv_name, decrypted);
            update_window(d);
            notify(d);
            pthread_mutex_unlock(&msg_mutex);
        }
    }
    return 0;
}

/*
 * Runs the client loop
 *
 * Params:
 * char sv_nameaddr[64]: Server address represented as a string
 * char nick[64]: The client nickname
 */
void client(char sv_nameaddr[64], char nick[64])
{
    strncpy(cl_name, nick, NAME_LEN-1);

    generate_bytes(key);
    generate_bytes(iv);
    int sv_fd = sock_setup(sv_nameaddr);
    printf("Connection established.\n");

    msgs_data d;
    d.curr = 0;
    d.size = 10; // Initially holds 10 messages
    d.msg_list = calloc(d.size, sizeof(msg));

    init_window();

    struct argl args;
    args.d = &d;
    args.sv_fd = sv_fd;

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
