#include "file_transfer_protocol.h"
#include "msocket.h"
#include "file_handler.h"
#include <string.h>
#include <stdlib.h>

int receive_data_part(int sfd, char* buf, int buf_size)
{
    int nread = 0;

    nread = socket_read(sfd, buf, buf_size);
    if (nread < 0)
    {
        return CANT_CONNECT;
    }
    return nread;
}

int receive_data(int sfd, char* data, int buf_size)
{
    int nread = 0;
    struct file_st file;
    char buf[BUF];
    //first receive the data metada
    nread = socket_read(sfd, buf, buf_size);
    if (nread < 0)
    {
        return CANT_CONNECT;
    }
    memcpy(&file, buf, nread);
    int data_received = 0;
    if(file.type == DIRECTORY_E)
    {
        if (create_dir(file.path) == false)
        {
            return CANT_CREATE_DIR;
        }
        return 0;
    }

    data = malloc(file.size);
    while (data_received < file.size)
    {
        nread = receive_data_part(sfd, buf, buf_size);
        if(nread < 0)
        {
            return CANT_CONNECT;
        }
        data_received += nread;
        memcpy(data+data_received, buf, nread);
    }
    if( data_received != file.size)
    {
        return WRONG_SIZE;
    }
    if (!create_file(file.path))
    {
        return CANT_CREATE_FILE;
    } 
    return data_received;
}

int transfer_data(int sfd, unsigned char* data, int data_size)
{
    int nwrite = 0;
    int data_sent = 0;
    int remain = 0;

    while (data_sent < data_size)
    {
        remain = data_size - data_sent;
        if (remain > BUF)
        {
            nwrite = socket_write(sfd, data+data_sent, BUF);
        }
        else 
        {
            socket_write(sfd, data+data_sent, remain);
        }
        if (nwrite < 0)
        {
            return WRONG_SIZE;
        }
        data_sent += nwrite;
    }

    if (data_sent != data_size)
    {
        return WRONG_SIZE;
    }
    return data_sent;
}
