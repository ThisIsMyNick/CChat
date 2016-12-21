#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "server.h"
#include "client.h"

int PORT = 31337;

void print_help()
{
    printf("Usage:\n"
            "CChat --conn (ip:port|name) [--nick yourname] [--help]\n");
    exit(1);
}

int main(int argc, char *argv[])
{
    char nick[64] = {};
    char sv_name[64] = {};

    const char* const short_options = "hc:n:";
    const struct option long_options[] =
    {
        {"help", no_argument      , NULL, 'h'},
        {"conn", required_argument, NULL, 'c'},
        {"nick", required_argument, NULL, 'n'},
        { NULL , NULL             , NULL,  0 }
    };

    int next_option;
    while ((next_option = getopt_long(argc, argv, short_options, long_options, NULL)) != -1)
    {
        switch (next_option)
        {
            case 'h':
                print_help();
                break;
            case 'c':
                strncpy(sv_name, optarg, 64);
                printf("Connect to server at %s\n", sv_name);
                break;
            case 'n':
                strncpy(nick, optarg, 64);
                printf("Nickname: %s\n", optarg);
                break;
            default:
                print_help();
        }
    }

    if (sv_name[0] == 0) //Run as server
    {
        printf("Running as server\n");
        server();
        return 0;
    }
    //Run as client
    client(sv_name, nick);
    return 0;
}
