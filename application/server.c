#include "server.h"
#include "file_handler.h"
#include "file_transfer_protocol.h"
#include "msocket.h"
#include <stdio.h>
#include <string.h>

int server(char *port, int buf_size, char* path)
{
    int sfd;
    int ret = 0;

#ifdef DEBUG
    printf("--------------- Initialized server side ------------------\n");
    printf("Host: %s",port);
    printf("----------------------------------------------------------\n");
#endif
    sfd = create_socket(port);
    if(sfd < 0)
    {
        printf("Error connecting to socket\n");
        return 1;
    }
    
    unsigned char* databuf = NULL;
    unsigned char clientbuf[buf_size];
    struct file_tree_st root;
    struct file_st* pfile = NULL;

    ret = read_dir(path, &root);
    if (ret < 0)
    {
        return ret;
    }

    print_file_tree(&root);

    printf("Send root file metadata\n");
    memcpy(clientbuf, &root, sizeof(struct file_tree_st));
    ret = socket_write(sfd, clientbuf, sizeof(struct file_tree_st));
    if (ret < 0)
    {
        printf("Error sending root file metadata\n");
        return ERROR;
    }

    printf("Start sending data\n");
    pfile = root.root;
    int data_sent = 0;
    while(data_sent < root.total_size)
    {
        if (pfile->type == FILE_E)
        {
            ret = get_file_bin(pfile, databuf);
            if (ret != 0)
            {
                printf("Error getting the file binary\n");
                return ERROR;
            }
        }
        ret = transfer_data(sfd, databuf, pfile->size);
        if (ret < 0)
        {
            printf("Error sending the data\n");
            return ERROR;
        }
        data_sent += ret;

    }

    return SUCCESS;
}
