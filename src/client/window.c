#include <fcntl.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "message.h"
#include "window.h"

WINDOW *input_win;
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
    while (i >= 0)
    {
        if (lines+msg_list[i].lines < max_y-2)
        {
            lines += msg_list[i].lines;
            starti = i;
        }
        else
            break;
        i--;
    }

    int x = starti;
    i = starti;
    for (;i < length; x+=msg_list[i].lines, i++)
    {
        mvprintw(x+1-starti, 0, "[%s] | <%s> %s", msg_list[i].timestamp, msg_list[i].user, msg_list[i].content);
    }
}

int get_max_x() {
    return max_x;
}

void draw_prompt()
{
    mvwprintw(input_win, 0, 0, "[Prompt] ");
    wrefresh(input_win);
}

void clear_msgs()
{
    int i;
    for (i = 1; i < max_y-2; ++i)
    {
        move(i,0);
        clrtoeol();
    }
}

void update_window(msgs_data *d)
{
    getmaxyx(stdscr, max_y, max_x); // re-calculate maxyx in case the window size changed
    int x,y;
    getyx(input_win, y, x);
    clear_msgs();
    draw_messages(d->msg_list, d->curr, d->curr);
    draw_prompt();
    wmove(input_win, y, x);
    refresh();
    wrefresh(input_win);

}

void get_input(char *dst)
{
    draw_prompt(); //to set position
    wgetnstr(input_win, dst, 255);
    mvwprintw(input_win, 0, 0, "%*s", max_x, " ");
}

void init_window()
{
    initscr();

    getmaxyx(stdscr, max_y, max_x);
    input_win = newwin(1,max_x, max_y-2,0);

    title_bar();

    draw_prompt();

    refresh();
    wrefresh(input_win);
}

void close_window()
{
    endwin();
    delwin(input_win);
}
