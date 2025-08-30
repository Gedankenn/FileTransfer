#include "file_handler.h"
#include "msocket.h"
#include "utils.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "server.h"
#include "client.h"

void menu(char *argv[])
{
    if (strcmp(argv[1], "-s") == 0)
    {
        printf("Entering server mode\n");
        server(argv[2], BUF);
    }
    if (strcmp(argv[1], "-c") == 0)
    {
        printf("Entering client mode\n");
        client(argv[2], BUF);
    }
    if (strcmp(argv[1], "-f") == 0)
    {
        printf("Listing files in dir\n");
        printf("Enter file path: ");
        struct file_tree_st file;
        read_dir(argv[2], &file);
    }
}

int main(int argc, char *argv[])
{
    int ret = 0;
    char ip[256];
    ret = find_my_ip(4, ip);
    if (ret == EXIT_FAILURE)
    {
        printf("Couldnt read ip\n");
        return ret;
    }
    printf("My ip: %s\n", ip);

    if (argc < 2)
    {
        printf("Usage: app <-s/-c/-f> <port>\n");
        return EXIT_SUCCESS;
    }
    printf("option: %s, port %s\n", argv[1], argv[2]);
    menu(argv);

    return EXIT_SUCCESS;
}
