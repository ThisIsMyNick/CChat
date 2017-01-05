#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "client.h"
#include "server.h"

int PORT = 31337;

void help_exit()
{
    printf("Usage:\n"
            "CChat --nick yourname [--conn (ip:port|name)] [--help]\n");
    exit(1);
}

int main(int argc, char *argv[])
{
    char nick[64] = {};
    char sv_nameaddr[64] = {};

    const char* const short_options = "hc:n:";
    const struct option long_options[] =
    {
        {"help", no_argument      , NULL, 'h'},
        {"conn", required_argument, NULL, 'c'},
        {"nick", required_argument, NULL, 'n'},
        { NULL , 0                , NULL,  0 }
    };

    int next_option;
    while ((next_option = getopt_long(argc, argv, short_options, long_options, NULL)) != -1)
    {
        switch (next_option)
        {
            case 'h':
                help_exit();
                break;
            case 'c':
                strncpy(sv_nameaddr, optarg, 64);
                printf("Connect to server at %s\n", sv_nameaddr);
                break;
            case 'n':
                strncpy(nick, optarg, 64);
                printf("Nickname: %s\n", optarg);
                break;
            default:
                help_exit();
        }
    }

    if (!*nick)
    {
        printf("Please specify nick.\n");
        help_exit();
    }

    if (sv_nameaddr[0] == 0) //Run as server
    {
        printf("Running as server\n");
        server(nick);
    } else
    {
        //Run as client
        client(sv_nameaddr, nick);
    }
    return 0;
}
