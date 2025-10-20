#include "server.h"
#include "file_handler.h"
#include "file_transfer_protocol.h"
#include "msocket.h"
#include "color.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#define RETRYES 3
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

static long int send_data(int sfd, struct file_st* pfile, int buf_size, long int data_send)
{
    long int ret = 0;
    int retryes = 0;
    char buf[buf_size];
    unsigned char* bin_file = NULL;
    // First send the file metadata 
    memcpy(buf, pfile, sizeof(struct file_st));
    #ifdef DEBUG
    printf("%s---------------------------------------------------\n", ANSI_YELLOW);
    printf("path: %s\n", pfile->path);
    printf("re_path: %s\n", pfile->re_path);
    if (pfile->type == FILE_E)
    {
        printf("type: FILE\n%s", ANSI_RESET);

    }
    else 
    {
        printf("type: Directory%s\n", ANSI_RESET);
    }
    #endif
    ret = socket_write(sfd, buf, sizeof(struct file_st));
    if (ret < 0)
    {
        printf("%sError sending file metadata%s\n",ANSI_RED,ANSI_RESET);
        return ret;
    }
    
    if (pfile->type == DIRECTORY_E)
    {
        if (pfile->childrem)
        {
            ret = send_data(sfd, pfile->childrem, buf_size, data_send);
            if (ret < 0)
            {
                return ret;
            }
        }

        if (pfile->brother)
        {
            ret = send_data(sfd, pfile->brother, buf_size, data_send);
            if (ret < 0)
            {
                return ret;
            }
        }
        data_send = data_send+ret;
        return data_send;
    }

    // if its file, send the file bin data
    printf("Sending file: %s\n", pfile->re_path);
    ret = get_file_bin(pfile, bin_file);
    if (ret < 0)
    {
        printf("%sError getting file bin%s\n",ANSI_RED,ANSI_RESET);
        return ret;
    }
    printf("file size: %li\n", pfile->size);

    ret = -1;
    while (retryes < RETRYES && ret < 0)
    {
        ret = transfer_data(sfd, bin_file, pfile->size);
        retryes++;
    }
    if (ret < 0)
    {
        printf("%sError sending file bin data%s\n",ANSI_RED,ANSI_RESET);
        return ret;
    }

    if (pfile->childrem)
    {
        ret = send_data(sfd, pfile->childrem, buf_size, data_send);
        if (ret < 0)
        {
            return ret;
        }
    }

    if (pfile->brother)
    {
        ret = send_data(sfd, pfile->brother, buf_size, data_send);
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

    printf("--------------- Initialized server side ------------------\n");
    printf("Port: %s\n",port);
    printf("----------------------------------------------------------\n");
    
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
    #ifdef DEBUG
    printf("%s ----------- SERVER DEBUG ----------------\n", ANSI_YELLOW);
    print_file_tree(&root);
    printf("root: %s:%d%s\n", root.root->path, root.root->type, ANSI_RESET);
    #endif


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
    long int data_sent = 0;
    while(data_sent < root.total_size)
    {
        data_sent = send_data(sfd, pfile, buf_size, data_sent);
        if (data_sent < 0)
        {
            printf("Error sending data\n");
            return ERROR;
        }
        printf("%sSent %li of %li%s\n", ANSI_GREEN, data_sent, root.total_size, ANSI_RESET);
    }

    if (data_sent != root.total_size)
    {
        printf("%sWrong size sent%s\n", ANSI_RED, ANSI_RESET);
        return ERROR;
    }
    return SUCCESS;
}
