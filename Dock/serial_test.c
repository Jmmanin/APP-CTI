#include "CommBridge.h"
#include <stdio.h>
#include <signal.h>

static volatile int run = 1;

void handler() //handles ^C
{
  run = 0;
}

int main()
{
    signal(SIGINT, handler);

    COMM_bridgeInit();
    
    printf("Monitor: %d\n", COMM_monitor());
    
    while(run)
    {
        serial_packet_t packet_recvd = COMM_getNextPacket();
        
        /printf("X = %f\n", packet_recvd.serial_orientation[0]);
        printf("Y = %f\n", packet_recvd.serial_orientation[1]);
        printf("Z = %f\n\n", packet_recvd.serial_orientation[2]);
    }
            
    COMM_closedown();
    
    return(0);
}
