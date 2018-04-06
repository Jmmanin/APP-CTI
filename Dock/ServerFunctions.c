/*The collection of tasks that the worker threads in the backend server will work on*/
#include <stdio.h>
#include <stdlib.h>

#include "FileAPI.h"
#include "UniversalDefines.h"

int validate_cmd(char cmd) {
    int i;
    int num_cmds = 5;
    char val_char[] = {'k', 'm', 'l', 'e', 'v'};

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
    printf("- 'l': (livestream) set up a line to the livestream output for the current incoming stream or\
     the next if none is currently running.\n");
    printf("- 'e': (end) return server standard input mode.\n");
    printf("- 'v': (view) view the contents of the file system state files.");
    
}
void T_data_transform(char *work_in, char *work_out, int num_pkts) {
    int i;
    char work_out_buff[MAX_PRDATBUFF_SIZE];
    //Example transform takes a 1 char input packet and puts the char twice into a char output packet
    for(i = 0; i < num_pkts; i++) {
        work_in[INP_PKT_SIZE*i] = work_out_buff[TRNS_PKT_SIZE*i];
        work_in[INP_PKT_SIZE*i] = work_out_buff[TRNS_PKT_SIZE*i + 1];
    }

    work_out = work_out_buff;
}