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
        return 1;
    }

	// ------------------ Connection states 
	// First, connect to the server 
	snprintf(buf, buf_size, "O PAI TA ON");
	socket_write(sfd, buf, strlen(buf));

	// Wait for root file metadata
	nread = socket_read(sfd,buf);
	if(nread < 0)
	{
		return 1;
	}
	memcpy(&root, buf, sizeof(struct file_tree_st));

#ifdef DEBUG
    printf("--------------- received root metadata -------------------\n");
    printf("Path: %s\n", root.root->path);
    printf("Size: %d\n", root.total_size);
    printf("Count: %d\n", root.files_count);
    printf("----------------------------------------------------------\n");
#endif

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

