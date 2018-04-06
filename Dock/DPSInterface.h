#ifndef DPS_INTERFACE_INCLUDE
#define DPS_INTERFACE_INCLUDE

int DPS_monitor();

int DPS_setup(int type, int sample_rate);

int DPS_setupRequestState();

int DPS_setupLivestreamState(int sample_rate);

int DPS_streamNextPacket(char *packet);

int DPS_getRequest();

int DPS_honorRequest();

void DPS_closedown();

#endif