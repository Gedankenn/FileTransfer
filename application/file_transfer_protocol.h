#ifndef __FILE_TRANSFER_PROTOCOL__
#define __FILE_TRANSFER_PROTOCOL__


enum file_transfer_errors
{
    OK = 0,
    WRONG_SIZE = -1,
    CANT_CREATE_DIR = -2,
    CANT_CREATE_FILE = -3,
    CANT_CONNECT = -4
};

int receive_data(int sfd, char* data);
int transfer_data(int sfd, unsigned char* data, int data_size);

#endif
