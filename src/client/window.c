#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define HEIGHT 22

typedef struct msg
{
    char *user;
    int padding;
    char *content;
    int lines;
} msg;

void title_bar()
{
    mvprintw(0,5, "CChat");
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

    int msg_start_offset = 8;
    for (i = starti; i < length; ++i)
    {
        mvprintw(i+1-starti, 0, "%s |", msg_list[i].user);
        mvprintw(i+1-starti, msg_start_offset, msg_list[i].content);
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

void fill3(msg* msgs)
{
    msgs[0].user = malloc(10*sizeof(char));
    strcpy(msgs[0].user, "User1");
    msgs[0].content = malloc(256*sizeof(char));
    strcpy(msgs[0].content, "Here's a message!");
    msgs[0].padding = 0;
    msgs[0].lines = 1;

    msgs[1].user = malloc(10*sizeof(char));
    strcpy(msgs[1].user, "User2");
    msgs[1].content = malloc(256*sizeof(char));
    strcpy(msgs[1].content, "Another message!");
    msgs[0].padding = 0;
    msgs[1].lines = 1;

    msgs[2].user = malloc(10*sizeof(char));
    strcpy(msgs[2].user, "User1");
    msgs[2].content = malloc(256*sizeof(char));
    strcpy(msgs[2].content, "Here's yet another!");
    msgs[0].padding = 0;
    msgs[2].lines = 1;
}

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
