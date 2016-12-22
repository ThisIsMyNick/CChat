#pragma once

typedef struct msg
{
    char *timestamp;
    char *user;
    char *content;
    int lines;
} msg;

void add_msg(msg *m, int *m_size, int *m_curr, char *user, char *content);
void free_msgs(msg *m, int curr);
