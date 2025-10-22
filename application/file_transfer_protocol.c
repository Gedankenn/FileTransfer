#include "file_transfer_protocol.h"
#include "msocket.h"
#include "file_handler.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "color.h"

int receive_data_part(int sfd, char* buf, int buf_size)
{
    int nread = 0;
    nread = socket_read(sfd, buf, buf_size);
    if (nread < 0)
    {
        return CANT_RECEIVE_BIN_DATA;
    }
    return nread;
}

int receive_data(int sfd, char* data, int buf_size)
{
    int nread = 0;
    struct file_st file;
    char buf[buf_size];
    //first receive the data metada
    #ifdef DEBUG
    printf("%sReceive data%s\n", ANSI_YELLOW, ANSI_RESET);
    #endif
    nread = socket_read(sfd, buf, buf_size);
    if (nread <= 0)
    {
        printf("Error receiving socket\n");
        return CANT_CONNECT;
    }
    memcpy(&file, buf, nread);
    if(file.type == DIRECTORY_E)
    {
        printf("directory: %s\n",file.re_path);
        if (create_dir(file.re_path) == false)
        {
            printf("Cant create path\n");
            return CANT_CREATE_DIR;
        }
        return 0;
    }
    printf("file: %s\n",file.path);
    printf("size: %li\n", file.size);

    data = malloc(file.size);
    long int data_received = 0;
    while (data_received < file.size)
    {
        nread = receive_data_part(sfd, buf, buf_size);
        #ifdef DEBUG
        printf("nread: %i\n", nread);
        #endif
        if(nread <= 0)
        {
            return CANT_RECEIVE_BIN_DATA;
        }
        data_received += nread;
        #ifdef DEBUG
        printf("data_received: %li\n", data_received);
        #endif
        memcpy(data+data_received, buf, nread);
    }
    if( data_received != file.size)
    {
        return WRONG_SIZE;
    }
    if (!create_file(file.path))
    {
        printf("Cant create file\n");
        return CANT_CREATE_FILE;
    } 
    #ifdef DEBUG
    printf("Received: %li\n",data_received);
    #endif
    return data_received;
}

int transfer_data(int sfd, unsigned char* data, long int data_size)
{
    int nwrite = 0;
    long int data_sent = 0;
    long int remain = 0;

    while (data_sent < data_size)
    {
        remain = data_size - data_sent;
        if (remain > BUF)
        {
            nwrite = socket_write(sfd, data+data_sent, BUF);
        }
        else 
        {
            nwrite = socket_write(sfd, data+data_sent, remain);
        }
        if (nwrite < 0)
        {
            return ERROR_WRITE_TO_SOCKET;
        }
        data_sent += nwrite;
        #ifdef DEBUG
        printf("sent %li of %li\n", data_sent, data_size);
        #endif
    }

    if (data_sent != data_size)
    {
        return WRONG_SIZE;
    }
    return data_sent;
}
