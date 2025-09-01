/**
 * File Transfer Finite State Machine
 *
 * A finite State machine for the file transfering protocol;
 *
 */

enum FSM_SERVER_ENUM{
	WAIT_CLIENT = 0,
	SEND_ROOT_METADADOS,
	SEND_DATA_METADATA,
	SEND_DATA,
    SERVER_END,
};

enum FSM_CLIENT_ENUM{
	CLIENT_JOIN = 0,
	RECEIVE_ROOT_METADATA,
	RECEIVE_DATA_METADATA,
	RECEIVE_DATA,
	CLIENT_END,
};



int serverFSM(int sfd, unsigned char* buf, int* fsm_state);
