#ifndef DPSCOMM_INCLUDE
#define DPSCOMM_INCLUDE
//header file for the DPS communication functions

void DPS_socketInit();

int DPS_getClient();

//does a communication with the client to establish what type of comm is occurring.
int DPS_getClientState();

int DPS_readStreamAddr(int *stream_id);

int DPS_returnStreamPayload(int payload_length, char *payload);

int DPS_sendPacket(char *pkt);

int DPS_check_release();

int DPS_close_port();

#endif