#include "server.h"
#include "file_handler.h"
#include "file_transfer_protocol.h"
#include "msocket.h"
#include "color.h"
#include <stdio.h>
#include <string.h>


int send_data(int sfd, struct file_st* pfile, int buf_size)
{
    int ret = 0;
    int data_send = 0;
    char buf[buf_size];
    unsigned char* bin_file = NULL;
    // First send the file metadata 
    memcpy(buf, pfile, sizeof(struct file_st));
    ret = socket_write(sfd, buf, sizeof(struct file_st));
    if (ret < 0)
    {
        printf("%sError sending file metadata%s\n",ANSI_RED,ANSI_RESET);
        return ret;
    }
    // if its file, send the file bin data
    ret = get_file_bin(pfile, bin_file);
    if (ret < 0)
    {
        printf("%sError getting file bin%s\n",ANSI_RED,ANSI_RESET);
        return ret;
    }

    ret = transfer_data(sfd, bin_file, sizeof(bin_file));
    if (ret < 0)
    {
        printf("%sError sending file bin data%s\n",ANSI_RED,ANSI_RESET);
        return ret;
    }
    if (pfile->childrem)
    {
        ret = send_data(sfd, pfile->childrem, buf_size);
        if (ret < 0)
        {
            return ret;
        }
    }
    data_send = data_send + ret;
    if (pfile->brother)
    {
        ret = send_data(sfd, pfile->brother, buf_size);
        if (ret < 0)
        {
            return ret;
        }
    }
    data_send = data_send+ret;
    return data_send;
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
    sfd = create_socket(port);
    if(sfd < 0)
    {
        printf("%sError connecting to socket\n%s", ANSI_RED, ANSI_RESET);
        return ERROR;
    }
    
    unsigned char clientbuf[buf_size];
    struct file_tree_st root;
    struct file_st* pfile = NULL;

    ret = read_dir(path, &root);
    if (ret == PATH_ERROR)
    {
        printf("%sWrong file path passed as parameter\n%s",ANSI_RED, ANSI_RESET);
        return ret;
    }
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
    if(pfile == NULL)
    {
        printf("%sError in data pointer pfile\n%s",ANSI_RED, ANSI_RESET);
        return ERROR;
    }
    int data_sent = 0;
    while(data_sent < root.total_size)
    {
        data_sent = send_data(sfd, pfile, buf_size);
    }

    if (data_sent != root.total_size)
    {
        printf("%sWrong size sent%s\n", ANSI_RED, ANSI_RESET);
    }
    return SUCCESS;
}
