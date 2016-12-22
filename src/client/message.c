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
void add_msg(msg *m, int *m_size, int *m_curr, char *user, char *content)
{
    if (*m_size == *m_curr)
    {
        m = realloc(m, (int)(*m_size*1.6)*sizeof(char));
        *m_size = (int)(*m_size*1.6);
    }
    m[*m_curr].timestamp = calloc(9, sizeof(char));
    strcpy(m[*m_curr].timestamp, "hh:mm:ss");
    m[*m_curr].user = calloc(11, sizeof(char));
    strncpy(m[*m_curr].user, user, 10);
    m[*m_curr].content = calloc(256, sizeof(char));
    strncpy(m[*m_curr].content, content, 255);
    m[*m_curr].lines = 1;

    *m_curr = *m_curr + 1;
}

void free_msgs(msg *m, int curr)
{
    int i;
    for (i = 0; i <= curr; ++i)
    {
        free(m[i].timestamp);
        free(m[i].user);
        free(m[i].content);
    }
    free(m);
}
