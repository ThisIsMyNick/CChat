#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "notify.h"
#include "message.h"

void notify(msgs_data *d)
{
    const char *disp = getenv("DISPLAY");
    if (!*disp)
        return;

    //http://superuser.com/a/533254
    FILE *fp = popen("xprop -id $(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2) _NET_WM_NAME", "r");
    char window[256] = {};
    fgets(window, 255, fp);
    window[strcspn(window, "\n")] = 0;
    pclose(fp);

    char *active = strchr(window, '=') + 1;
    if (strcmp(active, "./cchat") == 0
        || strcmp(active, "cchat") == 0)
    {
        return;
    }

#ifdef __linux__
    char s[256] = {};
    snprintf(s, 256, "notify-send \"CChat - %s\"", d->msg_list[d->curr-1].user);
    system(s);
#endif
}
