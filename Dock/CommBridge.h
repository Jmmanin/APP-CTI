// communication interface to the rig (comm bridge)

#ifndef COMM_BRIDGE_INCLUDE
#define COMM_BRIDGE_INCLUDE

//checks to see if there is a rig attempting to connect to the server
//      --returns:
//          --- 0: if there is nothing to connect to 
//          --- 1: if there is a rig looking to connect
int COMM_monitor();

//Assuming a rig is ready to connect (see COMM_monitor), takes the 
//  comm bridge through a set up stage to get ready for steady-state
//  packet transfer
int COMM_bridgeInit();

//Pulls down the next standard INP packet from the bridge
// Puts it in the provided buffer
int COMM_getNextPacket(char *payout_buff);

//Checks to see if the connection has been closed. Useful to check 
//  if a packet came back empty or garbage
//      --returns:
//          --- 0: if the connection is still active
//          --- 1: if the connection is down
int COMM_clientTerminated();

//Does a proper shutdown sequence with the communicating rig
void COMM_closedown();

#endif