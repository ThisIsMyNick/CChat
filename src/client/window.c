#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "message.h"
#include "window.h"

int max_x, max_y;

void title_bar()
{
    mvprintw(0,0, "CChat");
}

void draw_messages(msg *msg_list, int length, int display_to)
{
    int lines = 0;
    int starti;

    int i = display_to;
    while (--i >= 0)
    {
        if (lines+msg_list[i].lines <= max_y-1)
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

void draw_prompt()
{
    mvprintw(max_y-1, 0, "[Prompt] ");
}

void update_window(msg* m, int length)
{
    draw_messages(m, length, length);
    draw_prompt();
    refresh();
}

void get_input(char *dst)
{
    draw_prompt(); //to set position
    getnstr(dst, 255);
    mvprintw(max_y-1,0, "%*s", max_x, " ");
}

void init_window()
{
    initscr();

    getmaxyx(stdscr, max_y, max_x);

    title_bar();

    draw_prompt();

    refresh();
}

void close_window()
{
    endwin();
}
