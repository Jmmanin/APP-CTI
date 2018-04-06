#include "UniversalDefines.h"
#include <stdlib.h>
#include <stdio.h>

char datas[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
int curr_data = 0;

int COMM_closedown() {
    //does clean close down of bridge
    return 1;
}

int COMM_monitor() {
    return 1;
}

void COMM_bridgeInit(char *payload_buffer, int *samp_rt_ptr) {
    int sample_rate = 3;
    *samp_rt_ptr = sample_rate;
    payload_buffer = (char *) malloc(INP_PKT_SIZE * STD_TRANSP_TIME * sample_rate);
}

int COMM_getNextPacket(char *payout_buff) {
    //puts the transmitted packet in the payout buffer and returns
    //exit code has to do with availability of next packet
    payout_buff[0] = datas[curr_data];
    curr_data +=1;
    if(curr_data == 26) {
        curr_data = 0;
    }
    return 1;
}

int COMM_clientTerminated() {
    return 0;
}