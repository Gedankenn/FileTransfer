#include "server.h"
#include "file_handler.h"
#include "file_transfer_protocol.h"
#include "msocket.h"
#include <stdio.h>
#include <string.h>


int client_connected(int sfd, unsigned char* buf)
{
    int ret = 0;
    if (buf == NULL)
    {
        return ERROR;
    }
    ret = socket_read(sfd, buf);
    if (ret > 0)
    {
        buf[ret] = '\0';
        if (strcmp((const char *)buf, "O Pai ta On") == 0)
        {
            return SUCCESS;
        }
    }
    return ERROR;

}

int server(char *port, int buf_size, char* path)
{
    int sfd;
    int ret = 0;

#ifdef DEBUG
    printf("--------------- Initialized server side ------------------\n");
    printf("Host: %s",port);
    printf("----------------------------------------------------------\n");
#endif
    init_socket(buf_size, AF_UNSPEC, 0);
    sfd = create_socket(port);
    if(sfd < 0)
    {
        printf("Error connecting to socket\n");
        return 1;
    }
    
    unsigned char* buf = NULL;
    struct file_tree_st root;
    struct file_st* pfile = NULL;

    ret = read_dir(path, &root);
    if (ret < 0)
    {
        return ret;
    }

    // Wait for the client to connect 
    ret = client_connected(sfd, buf);
    if (ret != SUCCESS)
    {
        return ERROR;
    }

    pfile = root.root;
    int data_sent = 0;
    while(data_sent < root.total_size)
    {
        if (pfile->type == FILE_E)
        {
            ret = get_file_bin(pfile, buf);
            if (ret != 0)
            {
                return ERROR;
            }
        }
        ret = transfer_data(sfd, buf, pfile->size);
        if (ret < 0)
        {
            return ERROR;
        }
        data_sent += ret;

    }

    return SUCCESS;
}
