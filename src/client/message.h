#pragma once
#define MESSAGE_BUFFER_SIZE 1024
#define NAME_LEN 21

typedef struct packet
{
    char data[MESSAGE_BUFFER_SIZE];
    int length;
} packet;

typedef struct msg
{
    char *timestamp;
    char *user;
    char *content;
    int lines;
} msg;

typedef struct msgs_data
{
    struct msg* msg_list;
    int curr;
    int size;
} msgs_data;

void add_msg(msgs_data *d, char *user, char *content);
void free_msgs(msgs_data *d);
