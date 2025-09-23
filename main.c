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
    printf("Client: ./app -c <host> <port> <dest path>\n");
    printf("File testing: ./app -f <file path>\n");
    printf("App autotest: ./app -t\n");
    printf("------------------------------------------------\n");

}

void menu(char *argv[])
{
    int ret = 0;
    if (strcmp(argv[1], "-s") == 0)
    {
        printf("Entering server mode\n");
        ret = server(argv[2], 4095, argv[3]);
        if (ret == ERROR)
        {
            printf("Deu ruim \n");
        }
    }
    else if (strcmp(argv[1], "-c") == 0)
    {
        printf("Entering client mode\n");
        ret = client(argv[2], argv[3], 4095);
        if (ret == ERROR)
        {
            printf("Deu ruim \n");
        }
    }
    else if (strcmp(argv[1], "-f") == 0)
    {
        printf("Listing files in dir\n");
        struct file_tree_st file;
        read_dir(argv[2], &file);
    }
    else if (strcmp(argv[1], "-t") == 0)
    {
        printf("Auto Teste\n");
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
        menu_helper();
        return EXIT_SUCCESS;
    }
    menu(argv);

    return EXIT_SUCCESS;
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
