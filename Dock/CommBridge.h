// communication interface to the rig (comm bridge)

#ifndef COMM_BRIDGE_INCLUDE
#define COMM_BRIDGE_INCLUDE

#include <stdint.h>

#define START_VAL -1386103603 //start value for serial packet
#define PORT "/dev/ttyUSB0" //serial port

//Packet of data sent over xbees
//--angle order: pointer, middle, ring, little
//--pressure order: closest to little thumb to closest to little finger
//--orientation order: x, y, z
typedef struct serial_packet
{
  float serial_angle[4];
  float serial_pressure[5];
  float serial_orientation[3];
  int32_t serial_temp;
} serial_packet_t;

//checks if there is information in serial buffer
//--returns:
//--- 0: if there is no data
//--- non-zero: if there is data
int COMM_monitor();

//Opens serial connection to rig and generates orientation offsets
//Bases offsets on average orientations of first 5 packets received
//--returns:
//--- 0: if connection was successful
//--- 1: if connection was unsuccessful 
int COMM_bridgeInit();

//Gets next packet of information from serial
//Waits until start_val has been received and then reads packet
//--returns serial packet
serial_packet_t COMM_getNextPacket();

//Closes serial connection to rig 
//--returns:
//--- 0: if close was successful
//--- non-zero: if close was unsuccessful
int COMM_closedown();

#endif
