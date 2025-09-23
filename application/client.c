#include "client.h"
#include "file_handler.h"
#include "msocket.h"
#include "file_transfer_protocol.h"
#include <stdio.h>
#include <string.h>

#ifdef DEBUG
#include <stdlib.h>
#endif

int client(char* host, char* port, int buf_size)
{
    char buf[buf_size];
    int sfd = 0;
    int nread = 0;
	struct file_tree_st root;

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
        return ERROR;
    }

	// Wait for root file metadata
    printf("Waiting for root file metadata\n");
	nread = socket_read(sfd,buf);
	if(nread < 0)
	{
        printf("Didnt received answer from the server\n");
	    return ERROR;
	}
	memcpy(&root, buf, sizeof(struct file_tree_st));

    printf("--------------- received root metadata -------------------\n");
    printf("Path: %s\n", root.root->path);
    printf("Size: %d\n", root.total_size);
    printf("Count: %d\n", root.files_count);
    printf("----------------------------------------------------------\n");

	int received_data_size = 0;
	while(received_data_size < root.total_size)
	{
        nread = receive_data(sfd, buf);
        if (nread < 0)
        {
            return ERROR;
        }
        received_data_size += nread;
    }

    return 0;
}

