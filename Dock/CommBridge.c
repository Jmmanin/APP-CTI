#include "UniversalDefines.h"
#include "CommBridge.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

static float X_OFFSET = 0;
static float Y_OFFSET = 0;
static float Z_OFFSET = 0;

static int32_t start_buf = 0;
static FILE *f_ptr = NULL;

int COMM_closedown() {
	return(fclose(f_ptr));
}

int COMM_monitor() {
    int nread;
    
    ioctl(fileno(f_ptr), FIONREAD, &nread);
    
	return(nread);
}

int COMM_bridgeInit() {
	serial_packet_t temp;
	int packet_count = 0;
	float x_sum = 0, y_sum = 0, z_sum = 0;

	f_ptr = fopen(PORT, "r");
	
	if(f_ptr==NULL)
		return(1);
	else
	{
		while(packet_count<5)
		{
			temp = COMM_getNextPacket();
			
			x_sum += temp.serial_orientation[0];
			y_sum += temp.serial_orientation[1];
			z_sum += temp.serial_orientation[2];								
			
			packet_count++;
		}
		
		X_OFFSET = x_sum/packet_count;
		Y_OFFSET = y_sum/packet_count;
		Z_OFFSET = z_sum/packet_count;
		
		return(0);
	}
}

serial_packet_t COMM_getNextPacket() {
    serial_packet_t packet_to_recv;
	uint8_t byte_buf = 0;
	uint32_t temp = 0;
    
    if(start_buf == START_VAL)
    {
		fread(&packet_to_recv, sizeof(struct serial_packet), 1, f_ptr);

		packet_to_recv.serial_orientation[0] -= X_OFFSET;
		packet_to_recv.serial_orientation[1] -= Y_OFFSET;
		packet_to_recv.serial_orientation[2] -= Z_OFFSET;		

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
