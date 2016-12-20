#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

void print_help()
{
    printf("Usage:\n"
            "CChat (ip:port|name) [--nick yourname] [--help]\n");
}

int main(int argc, char *argv[])
{
    const char* const short_options = "hn:";
    const struct option long_options[] =
    {
        {"help", 0, NULL, 'h'},
        {"nick", 1, NULL, 'n'},
        { NULL , 0, NULL,  0 }
    };

    int next_option;
    while ((next_option = getopt_long(argc, argv, short_options, long_options, NULL)) != -1)
    {
        switch (next_option)
        {
            case 'h':
                print_help();
                break;
            case 'n':
                printf("Nickname: %s\n", optarg);
                break;
            default:
                print_help();
        }
    }

}
