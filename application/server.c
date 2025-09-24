#include "server.h"
#include "file_handler.h"
#include "file_transfer_protocol.h"
#include "msocket.h"
#include "color.h"
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
        printf("%sError connecting to socket\n%s", ANSI_RED, ANSI_RESET);
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

    printf("%sSend root file metadata\n%s", ANSI_GREEN, ANSI_RESET);
    memcpy(clientbuf, &root, sizeof(struct file_tree_st));
    ret = socket_write(sfd, clientbuf, sizeof(struct file_tree_st));
    if (ret < 0)
    {
        printf("%sError sending root file metadata\n%s",ANSI_RED, ANSI_RESET);
        return ERROR;
    }

    printf("%sStart sending data\n%s",ANSI_GREEN, ANSI_RESET);
    pfile = root.root;
    int data_sent = 0;
    while(data_sent < root.total_size)
    {
        if (pfile->type == FILE_E)
        {
            ret = get_file_bin(pfile, databuf);
            if (ret != 0)
            {
                printf("%sError getting the file binary\n%s",ANSI_RED, ANSI_RESET);
                return ERROR;
            }
        }
        ret = transfer_data(sfd, databuf, pfile->size);
        if (ret < 0)
        {
            printf("%sError sending the data\n%s",ANSI_RED, ANSI_RESET);
            return ERROR;
        }
        data_sent += ret;

    }

    return SUCCESS;
}
