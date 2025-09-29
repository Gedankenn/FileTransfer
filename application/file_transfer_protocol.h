#ifndef __FILE_TRANSFER_PROTOCOL__
#define __FILE_TRANSFER_PROTOCOL__

#define HELLO_MESSAGE "O Pai Ta On"

enum file_transfer_errors
{
    OK = 0,
    WRONG_SIZE = -1,
    ERROR_WRITE_TO_SOCKET = -2,
    CANT_CREATE_DIR = -3,
    CANT_CREATE_FILE = -4,
    CANT_CONNECT = -5,
    CANT_RECEIVE_BIN_DATA = -6,
};

int receive_data(int sfd, char* data, int buf_size);
int transfer_data(int sfd, unsigned char* data, int data_size);

#endif
