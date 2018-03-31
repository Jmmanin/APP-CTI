//A running instance of the server handling the docking functionalities
//The backend outter architecture is a process that commands 3 threads that manage incoming data,
//  internal transformation, and outgoing data buffering

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ServerFunctions.h"
//#include "InterfaceProtocols.h"

#define NUM_WORKERS 5
#define MSG_SIZE 16
#define DOCK_W comm_block[0]
#define TRNS_W comm_block[1]
#define DPS_W comm_block[2]
#define INP_W comm_block[3]
#define LIVS_W comm_block[4]

#define MONITOR_DELAY 0.25

//definitions
struct livestream_state {
    int go_live;
    int targetted_stream;
    int sample_rate;
} 
    //used to communicate between work threads
typedef struct thread_state {
    int targetted_stream;  //w->m
    int internal_stat;     //w->m
    int exit_code;         //w->m
    int shutdown;          //m->w
    char msg[MSG_SIZE];    //m<->w
    int read;              //m<->w associated to msg
} thread_state_t;

thread_state_t comm_block[NUM_WORKERS];
struct livestream_state live_state;

//Top Level Function Prototypes
void *dock_manager();
void *transform_manager();
void *dps_manager();
void *input_manager();

//Second Level Function Prototypes
void S_Init_CommBlock();
void S_flush_CommBlock(thread_state_t *target);
void S_View_Monitior();
void S_Create_livestream();
void S_End_livestream();


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
    if(pthread_create(&thread_collection[0], NULL, dock_manager, NULL)) {
        printf("Failed to create dock manager\n");
    }
    if(pthread_create(&thread_collection[1], NULL, transform_manager, NULL)) {
        printf("Failed to create transform manager\n");
    }
    if(pthread_create(&thread_collection[2], NULL, dps_manager, NULL)) {
        printf("Failed to create DPS manager\n");
    }
    if(pthread_create(&thread_collection[3], NULL, input_manager, NULL)) {
        printf("Failed to create input handler\n");
    }
    printf("Worker spawn complete\n");

    while(run) {
        //check status of the workers
        if(INP_W.exit_code == 1) {
            command = INP_W.msg[0]; //store the first char of the inp. buffer as the command char
        }

        //change configuration
        switch(command) {
            case 'm':
                S_View_Monitior();
                break;
            case 'k':
                run = 0;
                S_Send_Shutdown();
                break;
            case 'l':
                S_Create_livestream();
                break;
            case 'e':
                S_End_livestream();
                break;
        }
        
        if(INP_W.exit_code == 1) {
            S_flush_CommBlock(&INP_W);
            if(command != 'k') {
                pthread_create(&thread_collection[3], NULL, input_manager, NULL);
            }
        }

        //set this cycle to ~4Hz
        start_ti = clock();
        while(wait_ti < MONITOR_DELAY) {
            end_ti = clock();
            wait_ti = (double) (end_ti - start_ti) / CLOCKS_PER_SEC;
        }

        //reset loop vars
        wait_ti = 0;
        command = '\0';
    }

    //Close down protocols
    for(i = 1; i < NUM_WORKERS; i++) {
        printf("Waiting on worker %d to shut down.\n", i);
        pthread_join(&thread_collection[i], NULL);
    }
    printf("\nClosed Down. Bye!\n");
}

//************************
//       Managers
//************************
void *input_manager() {
    char command, clr; 
    int val = 0;
    
    INP_W.internal_stat = 1;

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
    INP_W.internal_stat = 0;
}

void *dock_manager() {
    while(DOCK_W.shutdown == 0);
}

void *transform_manager() {
    while(TRNS_W.shutdown == 0);
}

void *dps_manager() {
    while(DPS_W.shutdown == 0);
}

void *livestream_manager() {
    if(live_state.go_live == 0) {
        printf("LIVS LOG: Livestream created without being in live mode. Sleeping worker.\n");
        return;
    }


}

//*************************
//  King Thread Functions
//*************************
void S_Create_livestream() {
    if(live_state.go_live == 1) {
        printf("Mode is already in livestream. ")
        if(DOCK_W.targetted_stream != 0) {
            printf("Currently targetting id: %d\n\n", DOCK_W.targetted_stream);
        } else {
            printf("No stream is currently live.\n\n");
        }
        return;
    }

    Q_Init();
    live_state.targetted_stream = DOCK_W.targetted_stream;
    pthread_create(&thread_collection[4], NULL, livestream_manager, NULL);
}

void S_End_livestream() {
    if(live_state.go_live == 0) {
        printf("Dock is not currently attempting to stream.\n");
        return;
    }
    live_state.go_live = 0;
}

void S_View_Monitior() {
    int i;
    printf("\nWorker Arrangement: 0 <= Dock, 1 <= Transform, 2 <= DPS Manager, 3 <= Input Handler\n\n");
    for(i = 0; i < NUM_WORKERS; i++) {
        printf("Worker %d | Comm Block:\n", i);
        printf("-Target Stream: %d\n", comm_block[i].targetted_stream);
        printf("-Internal Status: %d\n", comm_block[i].internal_stat);
        printf("-Exit Code: %d\n", comm_block[i].exit_code);
        printf("-Current Message Buff: %s", comm_block[i].msg);
        printf("-Message Read: %d\n\n", comm_block[i].read);
    }
}

void S_Init_CommBlock(){
    //may need to init a few things not to 0 in the future but this works for now.
    int i;
    for(i = 0; i < NUM_WORKERS; i++) {
        S_flush_CommBlock(&comm_block[i]);
    }

    live_state.go_live = 0;
    live_state.targetted_stream = 0;
    live_state.sample_rate = 0;
}

void S_flush_CommBlock(thread_state_t *trgt) {
    int i;

    trgt->targetted_stream = 0;
    trgt->internal_stat = 0;
    trgt->shutdown = 0;
    trgt->exit_code = 0;
    trgt->read = 0;
    
    for(i = 0; i < MSG_SIZE; i++) {
        trgt->msg[i] = '\0';
    }
}
