//The outward facing interfacing calls and their functionalities
#include <stdlib.h>
#include <stdio.h>
#include "UniversalDefines.h"


int DPS_monitor() {
    return 1;
}

int DPS_setup(int type, int sample_rate) {
    return 0;
}

int DPS_setupRequestState() {
    return 0;
}

int DPS_setupLivestreamState(int sample_rate) {
    return 0;
}

int DPS_streamNextPacket(char *packet) {
    FILE *output = fopen("stream_output.txt", "a");
    fwrite(packet, TRNS_PKT_SIZE, 1, output);
    fclose(output);
    return 0;
}

int DPS_getRequest() {
    return 2;   
}

int DPS_honorRequest(int request_type) {
    if (request_type == 1) {
        return 0;
    }
    return 1;
}

void DPS_closedown() {}