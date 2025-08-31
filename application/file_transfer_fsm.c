#include "file_transfer_fsm.h"
#include <string.h>
#include "msocket.h"


int serverFSM(int sfd, unsigned char* buf, int* fsm_state)
{
    int nread = 0;
    switch (*fsm_state)
    {
        case METADADOS_GERAIS:
            socket_read(sfd, buf);
            return *fsm_state+1;
    
    }
    return 0;
}
