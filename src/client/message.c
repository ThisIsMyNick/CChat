#include <stdlib.h>
#include <string.h>
#include "message.h"

/*
 * add_msg - Add a message to list
 *
 * msg *m - Message list to modify (must be malloc'd)
 * int *m_size - Capacity of message list
 * int *m_curr - Current size of message list
 * char *user - Username of content author
 * char *content - Message content
 */
void add_msg(msgs_data *d, char *user, char *content)
{
    if (d->size == d->curr)
    {
        d->msg_list = realloc(d->msg_list, (int)(d->size*1.6)*sizeof(char));
        d->size = (int)(d->size*1.6);
    }
    d->msg_list[d->curr].timestamp = calloc(9, sizeof(char));
    strcpy(d->msg_list[d->curr].timestamp, "hh:mm:ss");
    d->msg_list[d->curr].user = calloc(11, sizeof(char));
    strncpy(d->msg_list[d->curr].user, user, 10);
    d->msg_list[d->curr].content = calloc(256, sizeof(char));
    strncpy(d->msg_list[d->curr].content, content, 255);
    d->msg_list[d->curr].lines = 1;

    d->curr++;
}

void free_msgs(msgs_data *d)
{
    int i;
    for (i = 0; i <= d->curr; ++i)
    {
        free(d->msg_list[i].timestamp);
        free(d->msg_list[i].user);
        free(d->msg_list[i].content);
    }
    free(d->msg_list);
}
