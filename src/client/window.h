#pragma once

typedef struct msg
{
    char *timestamp;
    char *user;
    int padding;
    char *content;
    int lines;
} msg;

void window();
