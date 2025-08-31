/**
 * File Transfer Finite State Machine
 *
 * A finite State machine for the file transfering protocol;
 *
 */

enum FSM_ENUM{
    METADADOS_GERAIS = 0,
    METADADOS,
    DADOS,
    END,
};



int serverFSM(int sfd, unsigned char* buf, int* fsm_state);
