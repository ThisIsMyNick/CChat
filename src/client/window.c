#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "window.h"
#define HEIGHT 22

void title_bar()
{
    mvprintw(0,0, "CChat");
}

void draw_messages(msg *msg_list, int length, int display_to)
{
    int lines = 0;
    int starti;

    int i = display_to-1;
    while (i-- >= 0)
    {
        if (lines+msg_list[i].lines <= HEIGHT)
        {
            lines += msg_list[i].lines;
            starti = i;
        }
        else
            break;
    }

    int max_user_len = 0;
    for (i = starti; i < length; ++i)
    {
        int len;
        if ((len = strlen(msg_list[i].user)) > max_user_len)
            max_user_len = len;
    }

    for (i = starti; i < length; ++i)
    {
        mvprintw(i+1-starti, 0, "[%s] %*s | %s", msg_list[i].timestamp, max_user_len,  msg_list[i].user, msg_list[i].content);
    }
}

void free_msgs(msg* msgs, int length)
{
    int i;
    for (i = 0; i < length; ++i)
    {
        free(msgs[i].content);
    }
}

#ifdef WINDOW_ONLY
void fill3(msg* msgs)
{
    msgs[0].timestamp = malloc(9*sizeof(char));
    strcpy(msgs[0].timestamp, "dd:mm:ss");
    msgs[0].user = malloc(10*sizeof(char));
    strcpy(msgs[0].user, "name1");
    msgs[0].content = malloc(256*sizeof(char));
    strcpy(msgs[0].content, "Here's a message!");
    msgs[0].padding = 0;
    msgs[0].lines = 1;

    msgs[1].timestamp = malloc(9*sizeof(char));
    strcpy(msgs[1].timestamp, "dd:mm:ss");
    msgs[1].user = malloc(10*sizeof(char));
    strcpy(msgs[1].user, "whataname");
    msgs[1].content = malloc(256*sizeof(char));
    strcpy(msgs[1].content, "Another message!");
    msgs[1].padding = 0;
    msgs[1].lines = 1;

    msgs[2].timestamp = malloc(9*sizeof(char));
    strcpy(msgs[2].timestamp, "dd:mm:ss");
    msgs[2].user = malloc(10*sizeof(char));
    strcpy(msgs[2].user, "defin");
    msgs[2].content = malloc(256*sizeof(char));
    strcpy(msgs[2].content, "Here's yet another!");
    msgs[2].padding = 0;
    msgs[2].lines = 1;
}
#endif

void window()
{
    initscr();

    title_bar();

    msg *msgs = malloc(3 * sizeof(msg));
    fill3(msgs);
    draw_messages(msgs, 3, 3);
    free_msgs(msgs, 3);

    refresh();
    getch();
    endwin();
}

#ifdef WINDOW_ONLY
int main()
{
    window();
}
#endif
