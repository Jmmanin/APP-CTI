//A running instance of the server handling the docking functionalities
//The backend outter architecture is a process that commands 3 threads that manage incoming data,
//  internal transformation, and outgoing data buffering

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "ServerFunctions.h"
#include "InterfaceProtocols.h"

//definitions
    //used to communicate between work threads
typedef struct thread_state {
    int targetted_stream;  //w->m
    int exit_code;         //w->m
    int shutdown;          //m->w
} thread_state_t;

//Top Level Function Prototypes
void *dock_manager();
void *transform_manager();
void *dps_manager();


int main(int argc, char *argv[]) {
    //Universal Management variables, buffers
    pthread_t thread_collection[3];
    char command;
    int run = 1;

    //main thread variables

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

    while(run) {
        printf("Server is running. Enter command to control operation: ");
        command = getchar();
        
        switch (command) {
            case 'k':
                //case kill
                printf("Shutting Down");
                break;
            default:
                printf("Command Not recognized. (Commands are 1 char inputs)\n");
        }
    }


    void *dock_manager() {

    }

    void *transform_manager() {
        int run
    }

    void *dps_manager() {

    }
}