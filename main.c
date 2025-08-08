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
        char file_path[FILE_PATH_SIZE];
        fgets(file_path, FILE_PATH_SIZE, stdin);
        int len = strlen(file_path);
        file_path[len-1] = '\0';
        struct file_st file;
        read_dir(file_path, &file);
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
        printf("Usage: app < -s/-c> <port>\n");
        return EXIT_SUCCESS;
    }
    printf("option: %s, port %s\n", argv[1], argv[2]);
    menu(argv);

    return EXIT_SUCCESS;
}
