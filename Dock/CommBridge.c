#include "UniversalDefines.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

static int32_t start_buf = 0;
static FILE *f_ptr = NULL;

int COMM_closedown() {
	return(fclose(f_ptr));
}

int COMM_monitor() {
	return(0);
}

int COMM_bridgeInit(char *payload_buffer, int *samp_rt_ptr) {
	f_ptr = fopen("/dev/cu.usbserial-DN03FU8O", "r");
	
	if(f_ptr==NULL)
		return(1);
	else
		return(0);
}

int COMM_getNextPacket(char *payout_buff) {
    serial_packet_t packet_to_recv;
	uint8_t byte_buf = 0;
	uint32_t temp = 0;
    
    if(start_recvd == start_val) //if start val received
    {
		fread(&packet_to_recv, sizeof(struct serial_packet), 1, f_ptr); //read packet

		return(packet_to_recv);
    }
    else //reads in bytes until start val is found
    {
      fread(&byte_buf, sizeof(uint8_t), 1, f_ptr); //reads byte
      start_buf = (uint32_t)start_buf >> 8; //shift inserts byte
      temp = (uint32_t)byte_buf << 24;
      start_buf = start_buf | temp;
    }
}

int COMM_clientTerminated() {
    return(COMM_closedown());
}