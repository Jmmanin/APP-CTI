/*The collection of tasks that the worker threads in the backend server will work on*/
#include <stdio.h>
#include <stdlib.h>

int validate_cmd(char cmd) {
    int i;
    int num_cmds = 4;
    char val_char[] = {'k', 'm', 'l', 'e'};

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
     the next if none is currentlt running.\n");
    printf("- 'e': (end) return server standard input mode.\n");
    
}
void Dock_fn() {}

void Transform_fn() {}

void ProcData_fn() {}