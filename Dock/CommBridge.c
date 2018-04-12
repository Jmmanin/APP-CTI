#include "UniversalDefines.h"
#include "CommBridge.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

static int32_t start_buf = 0;
static FILE *f_ptr = NULL;

int COMM_closedown() {
	return(fclose(f_ptr));
}

int COMM_monitor() {
    int nread;
    
    ioctl(fileno(f_ptr), FIONREAD, &nread);
    
    if(nread > 0)
        return(0);
    else
        return(1);
}

int COMM_bridgeInit() {
	f_ptr = fopen(PORT, "r");
	
	if(f_ptr==NULL)
		return(1);
	else
		return(0);
}

serial_packet_t COMM_getNextPacket() {
    serial_packet_t packet_to_recv;
	uint8_t byte_buf = 0;
	uint32_t temp = 0;
    
    if(start_buf == START_VAL)
    {
		fread(&packet_to_recv, sizeof(struct serial_packet), 1, f_ptr);

		return(packet_to_recv);
    }
    else
    {
      fread(&byte_buf, sizeof(uint8_t), 1, f_ptr);
      start_buf = (uint32_t)start_buf >> 8;
      temp = (uint32_t)byte_buf << 24;
      start_buf = start_buf | temp;
    }
}
