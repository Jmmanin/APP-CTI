//simulates the functionality of dps_manager in backServer.c
#include "UniversalDefines.h"
#include "DPSComm.h"
#include <stdio.h>
//#include "FileAPI.h"

char datas[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
int curr_data = 0;

typedef struct prdat_header { //copy of the header type
    int stream_id;
    int sample_rate;
    double run_time;
    int payload;
    int build_state;
} prdat_header_t;

int q_sim(char *pkt_buff);
int file_sim(int stream_id, char *outBuff);

int main() {
    int q_state, f_state, trgt_stream, client_up, release_state;
    int dps_servmode = -1;
    char pkt_buff[2];
    char proc_buff[MAX_PRDATBUFF_SIZE];
    prdat_header_t stream_header;
    
    DPS_socketInit();
    
    while(1) {
        switch(dps_servmode) {
            case -1:
                printf("Waiting on Client...\n");
                client_up = DPS_getClient();
                if(client_up) {
                    dps_servmode = 0;
                }
                continue;
            case 0:
                printf("Waiting on context...\n");
                dps_servmode = DPS_getClientState();
                printf("Client found, asking mode: %d\n", dps_servmode);
                break;
            case 1:
                printf("Asking client for stream id...");
                DPS_readStreamAddr(&trgt_stream);
                printf("given id: %d\n", trgt_stream);
                f_state = file_sim(trgt_stream, proc_buff);
                if(!f_state) {
                    stream_header.payload = 0;
                } else {
                    stream_header.payload = 18;
                }

                printf("Replaying with payload...");
                DPS_returnStreamPayload(stream_header.payload, proc_buff);
                printf("done.\n");
                break;
            case 2:
                q_state = q_sim(pkt_buff);
                printf("Got packet from queue. Status: %d, pkt: '%s'\n", q_state, pkt_buff);
                if(q_state == 1) {
                    printf("Transmitting paket...");
                    DPS_sendPacket(pkt_buff);
                    printf("done.\n");
                }
                break;
            default:
                dps_servmode = 0;
                continue;
        }
        printf("checking for mode release...");
        release_state = DPS_check_release();
        if (release_state == 1) {
            printf("released.\n");
            dps_servmode = 0;
        } else if (release_state == 2) {
            printf("released and closed.\n");
            dps_servmode = -1;
            DPS_close_port();
        }
        printf("\n");
    }
    printf("Broken out of server loop.\n");
}

int q_sim(char *pkt_buff) {
    if(curr_data >= 26) {
        curr_data = 0;
        return 0;
    }

    pkt_buff[0] = datas[curr_data];
    pkt_buff[1] = datas[curr_data];
    curr_data += 1;
    return 1;
}

int file_sim(int stream_id, char *outBuff) {
    char *outsource = "TestPayloadOutHere";
    int i;
    if(stream_id > 2 && stream_id < 5) {
        for(i = 0; i < 18; i++) {
            outBuff[i] = outsource[i];
        }
        return 1;
    }
    return 0;
}