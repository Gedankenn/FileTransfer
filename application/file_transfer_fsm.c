#include "file_transfer_fsm.h"
#include <string.h>
#include "msocket.h"


int serverFSM(int sfd, unsigned char* buf, int* fsm_state)
{
    int nread = 0;
    switch (*fsm_state)
    {
        case METADADOS_GERAIS: //wait here for the first connection
            nread = socket_read(sfd, buf); // program will stay stuck here waiting for socket
			if(nread > 0)
			{
				fsm_state++;
			}
            return nread;
    
    }
    return 0;
}
