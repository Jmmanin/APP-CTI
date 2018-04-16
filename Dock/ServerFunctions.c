/*The collection of tasks that the worker threads in the backend server will work on*/
#include <stdio.h>
#include <stdlib.h>

#include "FileAPI.h"
#include "UniversalDefines.h"
#include "CommBridge.h"

int validate_cmd(char cmd) {
    int i;
    int num_cmds = 3;
    char val_char[] = {'k', 'm', 'v'};

    for(i = 0; i < num_cmds; i++) {
        if(cmd == val_char[i]) {
            return 1;
        }
    }

    return 0;
}

void print_cmdls() {
    
    printf("\nTable of Server Control Commands:\n");
    printf("- 'k': (kill) close down server operations\n");
    printf("- 'm': (monitor) display current state of server\n");
    printf("- 'v': (view) view the contents of the file system state files.");
    
}
void T_data_transform(char *work_in, char *work_out, int num_pkts) {
    int i, j;
    //char work_out_buff[MAX_PRDATBUFF_SIZE];
    //Example transform takes a 1 char input packet and puts the char twice into a char output packet
    for(i = 0; i < num_pkts; i++) {
        for(j = 0; j < INP_PKT_SIZE; j++) {
            work_out[(INP_PKT_SIZE*i)+j] = work_in[(TRNS_PKT_SIZE*i)+j];
        }
    }

    //work_out = work_out_buff;
}

void Dock_convert2charArray(serial_packet_t *input, char *output) {
    int i;
    char *pkt_cp;
    pkt_cp = (char *) input;
    for(i = 0; i < INP_PKT_SIZE; i++) {
        output[i] = pkt_cp[i];
    }
}

/*typedef struct serial_packet
{
  float serial_angle[4];
  float serial_pressure[5];
  float serial_orientation[3];
  int32_t serial_temp;
} serial_packet_t;*/