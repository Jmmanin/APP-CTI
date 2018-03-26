/*The collection of tasks that the worker threads in the backend server will work on*/
#include <stdio.h>
#include <stdlib.h>

int validate_cmd(char cmd) {
    int i;
    int num_cmds = 2;
    char val_char[] = {'k', 'm'};

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
    
    return;
}
void Dock_fn() {}

void Transform_fn() {}

void ProcData_fn() {}