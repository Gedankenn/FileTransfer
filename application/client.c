#include "client.h"
#include <stdio.h>

int client(char* host, char* port, int buf_size)
{
    char buf[buf_size];
    int sfd = 0;

#ifdef DEBUG 
    printf("--------------- Initialized client side ------------------\n");
    printf("Host: %s:%s",host,port);
    printf("----------------------------------------------------------\n");
#endif
    init_socket(buf_size, AF_UNSPEC, 0);
    sfd = connect_socket(host, port);
    if(sfd < 0)
    {
        printf("Error connecting to socket\n");
        return 1;
    }


    return 0;
}

