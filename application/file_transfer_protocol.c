#include "file_transfer_protocol.h"
#include "msocket.h"
#include "file_handler.h"
#include <string.h>
#include <stdlib.h>

int receive_data_part(int sfd, char* buf, int buf_size)
{
    int nread = 0;

    nread = socket_read(sfd, buf);
    if (nread < 0)
    {
        return -1;
    }
    return nread;
}


int receive_data(int sfd, char* data, int data_size)
{
    int nread = 0;
    struct file_st file;
    char buf[BUF];
    //first receive the data metada
    nread = socket_read(sfd, buf);
    if (nread < 0)
    {
        return -1;
    }
    memcpy(&file, buf, nread);
    int data_received = 0;
    if(file.type == DIRECTORY_E)
    {
        if (create_dir(file.path) == false)
        {
            return -1;
        }
        return 0;
    }

    data = malloc(file.size);
    while (data_received < file.size)
    {
        nread = receive_data_part(sfd, buf, BUF);
        if(nread < 0)
        {
            return -1;
        }
        data_received += nread;
        memcpy(data+data_received, buf, nread);
    }
    return data_received;
}

int transfer_data(int sfd, char* data, int data_size)
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
            return -1;
        }
        data_sent += nwrite;
    }

    return data_sent;
}
