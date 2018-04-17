#include "CommBridge.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv)
{
    if(argc!=2) //checks for proper number of command line args
    {
        fprintf(stderr,"Usage: ./get_training_data <output filename>\n");
        return(1);
    }
    
    const int max_count = 300; //5 mins per file
    serial_packet_t sample_data;
    int sample_count;
    
    FILE *f_ptr = fopen(argv[1], "w");    
    
    COMM_bridgeInit();
    
    for(sample_count=0;sample_count<max_count;sample_count++)
    {
        sample_data = COMM_getNextPacket();
        fprintf(f_ptr, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", sample_data.serial_angle[0],
                                                                sample_data.serial_angle[1],
                                                                sample_data.serial_angle[2],
                                                                sample_data.serial_angle[3],
                                                                sample_data.serial_pressure[0],
                                                                sample_data.serial_pressure[1],
                                                                sample_data.serial_pressure[2],
                                                                sample_data.serial_pressure[3],
                                                                sample_data.serial_pressure[4],
                                                                sample_data.serial_orientation[0],
                                                                sample_data.serial_orientation[1],
                                                                sample_data.serial_orientation[2]);        
    }
                
    COMM_closedown();    

    fclose(f_ptr);
    
    return(0);
}
