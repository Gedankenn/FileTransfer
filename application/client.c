#include "client.h"
#include "file_handler.h"
#include "msocket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int client(char* host, char* port, int buf_size)
{
    char buf[buf_size];
    int sfd = 0;
    int nread = 0;
	struct file_tree_st root;
	struct file_st file;
	struct file_st* pfile;
	struct file_st* proot;
	proot = root.root;

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

	int received_data_size = 0;
	while(received_data_size < root.total_size)
	{
		nread = socket_read(sfd, buf);
		if(nread > 0)
		{
			memcpy(&file, buf, nread);
			pfile = malloc(sizeof(file));
			if(file.type == FILE_E)
			{
				received_data_size += file.size;
			}
		}
	}

    return 0;
}

