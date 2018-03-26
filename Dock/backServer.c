//A running instance of the server handling the docking functionalities
//The backend outter architecture is a process that commands 3 threads that manage incoming data,
//  internal transformation, and outgoing data buffering

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ServerFunctions.h"
//#include "InterfaceProtocols.h"

#define NUM_WORKERS 4
#define MSG_SIZE 16
#define DOCK_W comm_blok[0]
#define TRNS_W comm_block[1]
#define DPS_W comm_block[2]
#define INP_W comm_block[3]

#define MONITOR_RATE 0.25

//definitions
    //used to communicate between work threads
typedef struct thread_state {
    int targetted_stream;  //w->m
    int exit_code;         //w->m
    int shutdown;          //m->w
    char msg[MSG_SIZE];    //m<->w
    int read;              //m<->w associated to msg
} thread_state_t;

thread_state_t comm_block[NUM_WORKERS];

//Top Level Function Prototypes
void *dock_manager();
void *transform_manager();
void *dps_manager();
void *input_manager();

//Second Level Function Prototypes
void S_Init_CommBlock();
void S_flush_CommBlock(thread_state_t *target);
void S_Thread_Cmd_Relay(char cmd);


int main(int argc, char *argv[]) {
    //Universal Management variables, buffers
    pthread_t thread_collection[NUM_WORKERS];
    char command;
    int run = 1;
    clock_t start_ti, end_ti;
    double wait_ti = 0;

    //main thread variables
    S_Init_CommBlock();

    //thread set up
    /*if(pthread_create(&thread_collection[0], NULL, dock_manager, NULL)) {
        printf("Failed to create dock manager\n");
    }
    if(pthread_create(&thread_collection[1], NULL, transform_manager, NULL)) {
        printf("Failed to create transform manager\n");
    }
    if(pthread_create(&thread_collection[2], NULL, dps_manager, NULL)) {
        printf("Failed to create DPS manager\n");
    }*/
    if(pthread_create(&thread_collection[3], NULL, input_manager, NULL)) {
        printf("Failed to create input handler\n");
    }
    printf("Worker spawn complete\n");

    while(run) {
        //check status of the workers
        if(INP_W.exit_code == 1) {
            command = INP_W.msg[0]; //store the first char of the inp. buffer as the command char
            S_flush_CommBlock(&INP_W);
            S_Thread_Cmd_Relay(command);
            printf("MAIN THREAD: recieved '%c' as cmd", command);
            pthread_create(&thread_collection[3], NULL, input_manager, NULL);
        }
        //change configuration / fix old threads
        
        start_ti = clock();
        //wait .25 sec
        while(wait_ti < MONITOR_RATE) {
            end_ti = clock();
            wait_ti = (double) (end_ti - start_ti) / CLOCKS_PER_SEC;
        }
        wait_ti = 0;
    }
}
void *input_manager() {
    char command, clr; 
    int val = 0;
    
    while(!val) {
        printf("\nServer is running. Enter command to control operation: ");
        command = getchar(); //blocks until we get a char
        
        val = validate_cmd(command);
        if(!val) {
            print_cmdls();
        }
        while((clr = getchar()) != '\n' && clr != EOF) {} //clear input buffer
    }

    INP_W.exit_code = 1;
    INP_W.msg[0] = command;
}

void *dock_manager() {
    while(1);
}

void *transform_manager() {
    while(1);
}

void *dps_manager() {
    while(1);
}

void S_Thread_Cmd_Relay(char cmd) {

}

void S_Init_CommBlock(){

}

void S_flush_CommBlock(thread_state_t *trgt) {
    int i;

    trgt->targetted_stream = 0;
    trgt->shutdown = 0;
    trgt->exit_code = 0;
    trgt->read = 0;
    
    for(i = 0; i < MSG_SIZE; i++) {
        trgt->msg[i] = '\0';
    }
}
