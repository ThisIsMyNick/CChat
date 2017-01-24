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

/*
 * Render the title bar
 */
void title_bar()
{
    mvprintw(0,0, "CChat");
}

void break_lines(msg* msg_list, int curr)
{
    int i;
    for (i = 0; i < curr; ++i)
    {
        char s[50] = {};
        snprintf(s, 50, "[%s] <%s> ", msg_list[i].timestamp, msg_list[i].user);
        char content[256];
        strncpy(content, msg_list[i].content, 256);

        int lpanel_size = strlen(s);
        int msg_size = strlen(content);
        int lines = 1;
        while (lpanel_size + msg_size > max_x)
        {
            lines++;
            msg_size -= (max_x - lpanel_size);
        }
        //TODO: Only need to do on resize
        if (msg_list[i].plines)
        {
            int j;
            for (j = 0; j < msg_list[i].lines; ++j)
            {
                free(msg_list[i].plines[j]);
            }
            free(msg_list[i].plines);
        }
        char **plines;
        plines = malloc(lines*sizeof(char*));
        int j;
        for (j = 0; j < lines; ++j)
        {
            plines[j] = malloc(max_x+1);
            if (j == 0)
                sprintf(plines[j], "[%s] <%s> ", msg_list[i].timestamp, msg_list[i].user);
            else
                sprintf(plines[j], "%*s", lpanel_size, " ");
            strncat(plines[j], content+j*(max_x-lpanel_size), max_x-lpanel_size);
        }

        msg_list[i].lines = lines;
        msg_list[i].plines = plines;
    }
}

/*
 * Draw messages onto the screen
 *
 * Params:
 * msg *msg_list: List of messages to be drawn
 * int length: Number of messages to draw
 * int display_to: Index of the last message to draw
 */
void draw_messages(msg *msg_list, int length, int display_to)
{
    int lines = 0;
    int starti = 0;

    int i = display_to;
    while (i >= 0)
    {
        if (lines+msg_list[i].lines < max_y-1)
        {
            lines += msg_list[i].lines;
            starti = i;
        }
        else
            break;
        i--;
    }

    int x = starti;
    for (i = starti; i < display_to; ++i)
    {
        int j;
        for (j = 0; j < msg_list[i].lines && msg_list[i].plines; ++j)
        {
            mvprintw(x+1-starti, 0, "%s", msg_list[i].plines[j]);
            x++;
        }
    }
}

/*
 * Get the maximum x coordinate of the screen
 */
int get_max_x()
{
    return max_x;
}

/*
 * Draw the input prompt
 */
void draw_prompt()
{
    mvwprintw(input_win, 0, 0, "[Prompt] ");
    wrefresh(input_win);
}

/*
 * Clear the screen of all messages
 */
void clear_msgs()
{
    int i;
    for (i = 1; i < max_y-1; ++i)
    {
        move(i,0);
        clrtoeol();
    }
}

/*
 * Update the screen, redrawing messages and the prompt
 */
void update_window(msgs_data *d)
{
    int oldx = max_x, oldy = max_y;
    getmaxyx(stdscr, max_y, max_x); // re-calculate maxyx in case the window size changed
    if ((oldx != max_x) || (oldy != max_y))
    {
        if (max_x < 50)
            max_x = oldx;
        if (max_y < 3)
            max_y = oldy;
        mvwin(input_win, max_y-1, 0);
    }
    int x,y;
    getyx(input_win, y, x);
    clear_msgs();
    break_lines(d->msg_list, d->curr);
    draw_messages(d->msg_list, d->curr, d->curr);
    draw_prompt();
    wmove(input_win, y, x);
    refresh();
    wrefresh(input_win);
}

/*
 * Get input from the prompt window
 *
 * Params:
 * char *dst: Char array used to store the input
 */
void get_input(char *dst)
{
    draw_prompt(); //to set position
    wgetnstr(input_win, dst, 255);
    mvwprintw(input_win, 0, 0, "%*s", max_x, " ");
}

/*
 * Creates the initial display of the window
 */
void init_window()
{
    initscr();

    getmaxyx(stdscr, max_y, max_x);
    input_win = newwin(1,max_x, max_y-1,0);

    title_bar();

    draw_prompt();

    refresh();
    wrefresh(input_win);
}

/*
 * Closes the screen and removes both the main and input windows
 */
void close_window()
{
    endwin();
    delwin(input_win);
}
