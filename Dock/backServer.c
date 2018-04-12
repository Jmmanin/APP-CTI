//A running instance of the server handling the docking functionalities
//The backend outter architecture is a process that commands 3 threads that manage incoming data,
//  internal transformation, and outgoing data buffering

//Compile with: gcc backServer.c FileAPI.c CommBridge.c DPSInterface.c PacketQueue.c ServerFunctions.c -o <exe_name> -lpthread
//execute with ./<exe_name> 

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ServerFunctions.h"
#include "UniversalDefines.h"
#include "FileAPI.h"
#include "DPSInterface.h"
#include "CommBridge.h"
#include "PacketQueue.h"

//definitions
typedef struct live_state {
    int go_live;
    int stream_id;
    int sample_rate;
} livestream_state_t; 

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
livestream_state_t livestream_state;

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
void S_Send_Shutdown();


int main(int argc, char *argv[]) {
    //Universal Management variables, buffers
    pthread_t thread_collection[NUM_WORKERS];
    char command;
    int run = 1;
    int i;
    clock_t start_ti, end_ti;
    double wait_ti = 0;

    //main thread variables
    S_Init_CommBlock();
    FS_Init();

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
        while(wait_ti < SVR_MONITORING_DELAY) {
            end_ti = clock();
            wait_ti = (double) (end_ti - start_ti) / CLOCKS_PER_SEC;
        }

        //reset loop vars
        wait_ti = 0;
        command = '\0';
    }

    //Close down protocols
    for(i = 0; i < NUM_WORKERS; i++) {
        printf("Waiting on worker %d to shut down.\n", i);
        pthread_join(thread_collection[i], NULL);
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
    int i;
    int max_pkts, miss_pkts;
    int samp_rate = 1;
    int rig_active = 0;
    int new_stream_flag = 1;
    
    clock_t start_ti, end_ti, wait_ti;

    char *holdBuff;
    char curr_pkt_ser;
    serial_packet_t curr_pkt;

    printf("data pkt struct size: %d", sizeof(serial_packet_t));
    Q_Init(INP_PKT_SIZE);  // creating instance of live transfer q
    curr_pkts = 0;
    holdBuff = (char *) malloc(MAX_TRFBUFF_SIZE);

    while(DOCK_W.shutdown == 0) {
        if(!rig_active) {
            rig_active = COMM_bridgeInit();
            if(rig_active && new_stream_flag) {
                DOCK_W.targetted_stream = create_new_rawstream(samp_rate);
                livestream_state.stream_id = DOCK_W.targetted_stream;
                livestream_state.sample_rate = samp_rate;
                new_stream_flag = 0;
            }
        } else {
            if(!COMM_monitor()) {
                curr_pkt = COMM_getNextPacket();
                Dock_convert2charArray(serial_packet_t *curr_pkt, char *curr_pkt_ser);
                
                if(livestream_state.go_live) {
                    Q_addData(curr_pkt_ser);
                }
                
                for(i = 0; i < INP_PKT_SIZE; i++) {
                    holdBuff[(curr_pkts*INP_PKT_SIZE) + i] = curr_pkt_ser[i];
                }
                curr_pkts += 1;

               if(curr_pkts >= (MAX_TRFBUFF_SIZE / INP_PKT_SIZE)) {
                    store_raw_chunk(DOCK_W.targetted_stream, holdBuff, (MAX_TRFBUFF_SIZE / INP_PKT_SIZE));
                    curr_pkts = 0;
                }

            } else {
                miss_pkts += 1;
            }
        }

        if(miss_pkts > DOCK_MISS_TRIGGER) {
            if(curr_pkts > 0) {
                store_raw_chunk(DOCK_W.targetted_stream, holdBuff, curr_pkts);
            }
            cap_rawstream(DOCK_W.targetted_stream);

            DOCK_W.targetted_stream = 0;
            curr_pkts = 0;
            rig_active = 0;
            new_stream_flag = 1;
        }

        start_ti = clock();   
        while(wait_ti < 0.33) {
            end_ti = clock();
            wait_ti = (double) (end_ti - start_ti) / CLOCKS_PER_SEC;
        }
        wait_ti = 0;
    }

    if(rig_active) {
        if(curr_pkts > 0) {
            store_raw_chunk(DOCK_W.targetted_stream, holdBuff, curr_pkts);
        }
        cap_rawstream(DOCK_W.targetted_stream);
    }
    COMM_closedown();
    livestream_state.go_live = 0;
    free(holdBuff);
}

void *transform_manager() {
    TRNS_W.targetted_stream = 0;
    int stream_id = 0; //internal stream id prevents external modifications
    int output_payload;
    trf_header_t work_meta;
    char *workload_raw;
    char *workload_proc;
    int i;
    printf("Transform manager up");
    while(TRNS_W.shutdown == 0) {
        stream_id = checkout_raw_chunk(stream_id, workload_raw, &work_meta);
        if(stream_id != 0) {
            printf("\nChecking out chunk: stream: %d\n", stream_id);
            printf("time: %lf, payload, %s", work_meta.time_slice, workload_raw);
        }
        for(i = 0; i < work_meta.payload; i++) {
            printf("%c", workload_raw[i]);
        }
        TRNS_W.targetted_stream = stream_id;
        
        if(stream_id != 0) {
            output_payload = (work_meta.payload / INP_PKT_SIZE) * TRNS_PKT_SIZE;
            T_data_transform(workload_raw, workload_proc, (work_meta.payload/INP_PKT_SIZE));
            store_processed_chunk(stream_id, workload_proc, output_payload);
        }
    }
    TRNS_W.exit_code = 1;
}

void *dps_manager() {
    trf_header_t transform_guide;
    int state = 0;
    int response = 0;
    int ack;
    char curr_pkt[INP_PKT_SIZE];
    char sendout_pkt[TRNS_PKT_SIZE];

    while(!DPS_W.shutdown) {
        if(state == 0) {
            state = DPS_monitor();
            DPS_setup(state, livestream_state.sample_rate); //calls the right type of setup
        }
        if(state == 1) { //single request i/o
            response = DPS_getRequest(); //blocking?
            DPS_honorRequest(response);
            if(response == -1) { //whatever response code means client wants to shift to livestream mode
                DPS_setupLivestreamState(livestream_state.sample_rate);
                //Q_reset(); //do we want to do that
                livestream_state.go_live = 1;
                state == 2;
            }
            if(response == -2) { //whatever response code means client would like to close
                DPS_closedown();
                state = 0;
            }
        } else if(state == 2 && livestream_state.go_live == 1) { //livestream i/o
            Q_removeData(curr_pkt);
            T_data_transform(curr_pkt, sendout_pkt, 1);
            ack = DPS_streamNextPacket(sendout_pkt);
            if(!ack) {
                DPS_setupRequestState();
                state = 1;
            }
        }
    }
}

void *livestream_manager() {
    if(livestream_state.go_live == 0) {
        printf("LIVS LOG: Livestream created without being in live mode. Sleeping worker.\n");
    }


}

//*************************
//  King Thread Functions
//*************************
void S_Create_livestream() {
    if(livestream_state.go_live == 1) {
        printf("Mode is already in livestream. ");
        if(DOCK_W.targetted_stream != 0) {
            printf("Currently targetting id: %d\n\n", DOCK_W.targetted_stream);
        } else {
            printf("No stream is currently live.\n\n");
        }
        return;
    } 

    printf("Server is now prepared to livestream from this point.\n");
    Q_reset();
    livestream_state.stream_id = DOCK_W.targetted_stream;
    livestream_state.go_live = 1;

    //pthread_create(&thread_collection[4], NULL, livestream_manager, NULL);
}

void S_End_livestream() {
    if(livestream_state.go_live == 0) {
        printf("Dock is not currently attempting to stream.\n");
        return;
    }
    printf("Live stream mode turned off");
    livestream_state.go_live = 0;
}

void S_View_Monitior() {
    int i;

    printf("Livestream state struct:\n");
    printf("-Currently in live mode: %d\n", livestream_state.go_live);
    printf("-Stream identifier: %d\n", livestream_state.stream_id);
    printf("-Stream sample rate: %d\n", livestream_state.sample_rate);

    printf("\nWorker Arrangement: 0 <= Dock, 1 <= Transform, 2 <= DPS Manager, 3 <= Input Handler\n\n");
    for(i = 0; i < NUM_WORKERS; i++) {
        printf("Worker %d | Comm Block:\n", i);
        printf("-Target Stream: %d\n", comm_block[i].targetted_stream);
        printf("-Internal Status: %d\n", comm_block[i].internal_stat);
        printf("-Exit Code: %d\n", comm_block[i].exit_code);
        printf("-Current Message Buff: %s\n", comm_block[i].msg);
        printf("-Message Read: %d\n\n", comm_block[i].read);
    }
}

void S_Init_CommBlock(){
    //may need to init a few things not to 0 in the future but this works for now.
    int i;
    for(i = 0; i < NUM_WORKERS; i++) {
        S_flush_CommBlock(&comm_block[i]);
    }

    livestream_state.go_live = 0;
    livestream_state.stream_id = 0;
    livestream_state.sample_rate = 0;
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

void S_Send_Shutdown() {
    int i;
    for(i = 0; i < NUM_WORKERS; i++) {
        comm_block[i].shutdown = 1;
    }
}
