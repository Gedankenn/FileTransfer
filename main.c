#include "file_handler.h"
#include "utils.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "server.h"
#include "client.h"
#include <string.h>

void menu_helper(void)
{
    printf("---------------- HELPER MENU -------------------\n");
    printf("Server: ./app -s <port> <file path>\n");
    printf("Client: ./app -c <host> <port> [dest path, default .]\n");
    printf("File testing: ./app -f <file path>\n");
    printf("App autotest: ./app -t\n");
    printf("------------------------------------------------\n");

}

int menu(int argc, char *argv[])
{
    int ret = 0;
    if (strcmp(argv[1], "-s") == 0 && argc >= 4)
    {
        printf("Entering server mode\n");
        ret = server(argv[2], 4095, argv[3]);
        if (ret == ERROR)
        {
            printf("Deu ruim \n");
        }
    }
    else if (strcmp(argv[1], "-c") == 0 && argc >= 4)
    {
        printf("Entering client mode\n");
        ret = client(argv[2], argv[3], 4095, argc >= 5 ? argv[4] : ".");
        if (ret == ERROR)
        {
            printf("Deu ruim \n");
        }
    }
    else if (strcmp(argv[1], "-f") == 0 && argc >= 3)
    {
        printf("Listing files in dir\n");
        struct file_tree_st file;
        read_dir(argv[2], &file);
    }
    else if (strcmp(argv[1], "-t") == 0 && argc >= 3)
    {
        printf("Auto Teste\n");
        struct file_tree_st file;
        read_dir(argv[2], &file);
    }
    else
    {
        menu_helper();
        return ERROR;
    }
    return ret;
}

int main(int argc, char *argv[])
{
    int ret = 0;
    char ip[256];
    ret = find_my_ip(4, ip);
    if (ret == EXIT_FAILURE)
    {
        printf("Couldnt read ip\n");   /* informational only */
    }
    else
    {
        printf("My ip: %s\n", ip);
    }

    if (argc < 2)
    {
        menu_helper();
        return EXIT_SUCCESS;
    }
    ret = menu(argc, argv);

    return ret == SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}


/**
┌──────┐                                ┌──────┐
│SERVER│                                │CLIENT│
└──┬───┘                                └──┬───┘
   │             Request File              │    
   │ ◄─────────────────────────────────────┤    
   │                                       │    
   │           MetaDados  Geral            │    
   ├─────────────────────────────────────► │    
   │                                       │    
   │                  ACK                  │    
   │ ◄─────────────────────────────────────┤    
   │                                       │    
   │         File/Folder MetaDados         │    
   ├─────────────────────────────────────► │    
   │                                       │    
   │           File/Folder Dados           │    
   ├─────────────────────────────────────► │    
   │                                       │    
   │                 ACK                   │    
   │ ◄─────────────────────────────────────┤    
   │                                       │    
   │                 END                   │    
   ├─────────────────────────────────────► │    
   ▼                                       ▼    
 */
