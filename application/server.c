#include "server.h"
#include "file_handler.h"
#include "msocket.h"
#include <stdio.h>
#include "file_transfer_fsm.h"

int server(char* host, char *port, int buf_size)
{
    int sfd = 0;
    int fsm = 0;
    int ret = 0;
#ifdef DEBUG
    printf("--------------- Initialized server side ------------------\n");
    printf("Host: %s:%s",host,port);
    printf("----------------------------------------------------------\n");
#endif
    init_socket(buf_size, AF_UNSPEC, 0);
    sfd = create_socket(port);
    if(sfd < 0)
    {
        printf("Error connecting to socket\n");
        return 1;
    }
    
    unsigned char buf[buf_size];

    while(ret != 0)
    {
        ret = serverFSM(sfd, buf, &fsm);
    }

    return SUCCESS;
}
