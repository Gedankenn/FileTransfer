#include "client.h"
#include "file_handler.h"
#include "msocket.h"
#include "file_transfer_protocol.h"
#include "color.h"
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
    sfd = connect_socket(host, port);
    if(sfd < 0)
    {
        printf("%sError connecting to socket\n%s",ANSI_RED, ANSI_RESET);
        return ERROR;
    }

	// Wait for root file metadata
    printf("%sWaiting for root file metadata\n%s",ANSI_GREEN, ANSI_RESET);
	nread = socket_read(sfd, buf, buf_size);
	if(nread < 0)
	{
        printf("%sDidnt received the root file metadata from the server\n%s",ANSI_RED, ANSI_RESET);
	    return ERROR;
	}
	memcpy(&root, buf, sizeof(struct file_tree_st));

    printf("%s--------------- received root file metadata -------------------\n",ANSI_MAGENTA);
    print_total_size(root.total_size);
    printf("Files count: %d\n", root.files_count);
    printf("Folders count: %d\n", root.folders_count);
    printf("----------------------------------------------------------\n%s", ANSI_RESET);

	int received_data_size = 0;
	while(received_data_size < root.total_size)
	{
        nread = receive_data(sfd, buf, buf_size);
        if (nread < 0)
        {
            return ERROR;
        }
        received_data_size += nread;
    }

    return 0;
}

