#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "message.h"
#include "window.h"

/*
 * Add a message to the list of messages
 *
 * msg *m - Message list to modify (must be malloc'd)
 * int *m_size - Capacity of message list
 * int *m_curr - Current size of message list
 * char *user - Username of content author
 * char *content - Message content
 */
void add_msg(msgs_data *d, char *user, char *content)
{
    // Reallocate memory if necessary
    if (d->size-1 == d->curr)
    {
        d->size *= 2;
        d->msg_list = realloc(d->msg_list, d->size*sizeof(msg));
    }
    d->msg_list[d->curr].timestamp = calloc(9, sizeof(char));

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(d->msg_list[d->curr].timestamp, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);

    d->msg_list[d->curr].user = calloc(11, sizeof(char));
    strncpy(d->msg_list[d->curr].user, user, 10);
    d->msg_list[d->curr].content = calloc(256, sizeof(char));
    strncpy(d->msg_list[d->curr].content, content, 255);
    int len = snprintf(NULL, 0, "[%s] <%s> %s", d->msg_list[d->curr].timestamp, d->msg_list[d->curr].user, d->msg_list[d->curr].content);
    d->msg_list[d->curr].plines = 0;
    d->msg_list[d->curr].lines = (len / get_max_x()) + 1;

    d->curr++;
}

/*
 * Frees the msgs_data struct and everything in it
 */
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
