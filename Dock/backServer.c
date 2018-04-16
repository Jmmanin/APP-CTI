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
#include "DPSComm.h"
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
void S_View_fileSystem();
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
    livestream_state.go_live = 0;

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
    printf("Worker spawn complete.\n");

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
            case 'v':
                S_View_fileSystem();
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
        printf("Waiting on worker %d to shut down....", i);
        pthread_join(thread_collection[i], NULL);
        printf(" thread closed.\n");
    }
    printf("\nServer Closed Down. Bye!\n");
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
    int rig_active = 1;  //Active low
    int new_stream_flag = 1;
    
    clock_t start_ti, end_ti, wait_ti;

    char *holdBuff;
    char curr_pkt_ser;
    serial_packet_t curr_pkt;

    //printf("data pkt struct size: %d", sizeof(serial_packet_t));
    Q_Init(INP_PKT_SIZE);  // creating instance of live transfer q
    curr_pkts = 0;
    holdBuff = (char *) malloc(MAX_TRFBUFF_SIZE);

    while(DOCK_W.shutdown == 0) {
        if(rig_active) {
            rig_active = COMM_bridgeInit();
            if(!rig_active && new_stream_flag) {
                DOCK_W.targetted_stream = create_new_rawstream(samp_rate);
                livestream_state.stream_id = DOCK_W.targetted_stream;
                livestream_state.sample_rate = samp_rate;
                new_stream_flag = 0;
            }
        } else {
            if(!COMM_monitor()) {
                curr_pkt = COMM_getNextPacket();
                Dock_convert2charArray(&curr_pkt, curr_pkt_ser);
                
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
                miss_pkts = 0;

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
            rig_active = 1;
            new_stream_flag = 1;
        }

        start_ti = clock();   //polls for packets at ~3Hz
        while(wait_ti < 0.33) {
            end_ti = clock();
            wait_ti = (double) (end_ti - start_ti) / CLOCKS_PER_SEC;
        }
        wait_ti = 0;
    }

    if(!rig_active) {
        if(curr_pkts > 0) {
            store_raw_chunk(DOCK_W.targetted_stream, holdBuff, curr_pkts);
        }
        cap_rawstream(DOCK_W.targetted_stream);
    }
    COMM_closedown();
    free(holdBuff);
    DOCK_W.exit_code = 1;
}

void *transform_manager() {
    TRNS_W.targetted_stream = 0;
    int stream_id = 0; //internal stream id prevents external modifications
    int output_payload;
    trf_header_t work_meta;
    char workload_raw[MAX_TRFBUFF_SIZE];
    char workload_proc[MAX_PRDATBUFF_SIZE];
    int i;

    while(TRNS_W.shutdown == 0) {
        stream_id = checkout_raw_chunk(stream_id, workload_raw, &work_meta);
        /*if(stream_id != 0) {
            printf("\nChecking out chunk: stream: %d\n", stream_id);
            printf("time: %lf, payload, %s", work_meta.time_slice, workload_raw);
        }
        for(i = 0; i < work_meta.payload; i++) {
            printf("%c", workload_raw[i]);
        }*/
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
    int q_state, f_state, trgt_stream, client_up, release_state;
    int dps_servmode = -1;
    char pkt_buff[2];
    char proc_buff[MAX_PRDATBUFF_SIZE];
    prdat_header_t stream_header;
    
    DPS_socketInit();
    Q_Init(INP_PKT_SIZE);
    
    while(1) {
            //check for shutdown command.
        if(DPS_W.shutdown == 1) {
            DPS_close_port();
            break;
        }
            //jump to actions associated with current state
        switch(dps_servmode) {
                    //opening new connection
            case -1:
                //printf("Waiting on Client...\n");
                client_up = DPS_getClient();
                if(client_up) {
                    dps_servmode = 0;
                }
                continue;
                    //getting the transfer mode context 
            case 0:
                //printf("Waiting on context...\n");
                dps_servmode = DPS_getClientState();
                //printf("Client found, asking mode: %d\n", dps_servmode);
                if(dps_servmode == 2) {
                    Q_reset();
                    livestream_state.go_live = 1;
                }
                break;
                    //reading a specific stream by id
            case 1:
                //printf("Asking client for stream id...");
                DPS_readStreamAddr(&trgt_stream);
                //printf("given id: %d\n", trgt_stream);
                f_state = file_sim(trgt_stream, proc_buff);
                if(!f_state) {
                    stream_header.payload = 0;
                } else {
                    stream_header.payload = 18;
                }

                //printf("Replaying with payload...");
                DPS_returnStreamPayload(stream_header.payload, proc_buff);
                //printf("done.\n");
                break;
                    //get streaming packets
            case 2:
                q_state = Q_removeData();
                //printf("Got packet from queue. Status: %d, pkt: '%s'\n", q_state, pkt_buff);
                if(q_state == 1) {
                    //printf("Transmitting paket...");
                    DPS_sendPacket(pkt_buff);
                    //printf("done.\n");
                }
                break;
            default:
                dps_servmode = 0;
                continue;
        }
        //printf("checking for mode release...");
        release_state = DPS_check_release();
        if (release_state == 1) {
            //printf("released.\n");
            dps_servmode = 0;
        } else if (release_state == 2) {
            //printf("released and closed.\n");
            dps_servmode = -1;
            DPS_close_port();
        }
        if(release_state != 0) {
            livestream_state.go_live = 0;
        }
        //printf("\n");
    }
    DPS_W.exit_code = 1;
}

//*************************
//  King Thread Functions
//*************************

void S_View_Monitior() {
    int i;

    printf("Livestream state struct:\n");
    printf("-Currently in live mode: %d\n", livestream_state.go_live);
    printf("-Target stream identifier: %d\n", livestream_state.stream_id);
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

void S_View_fileSystem() {
    FS_Init();

    FILE *st, *fs, *rq;
    int st_buff;
    char fs_buff[1024];
    int rq_buff;
    int reading = 1;
    
    //state table
    printf("Current File System State\n");
    printf("----State Table-----\n");
    st = fopen(STATE_TABLE, "rb");
    while(reading != 0) {
        reading = fread(&st_buff, sizeof(int), 1, st);
        if(reading != 0) {
            printf("%d\n", st_buff);
        }
    }

    //overview table
    printf("\n---fs system table---\n");
    reading = 1;
    fs = fopen(FS_TABLE, "r");
    reading = fread(fs_buff, sizeof(char), 1024, fs);
    printf("Bytes present: %d\n", reading);
    printf("payload: %s\n", fs_buff); 

    //raw queue 
    printf("\n---raw work queue---\n");
    reading = 1;
    rq = fopen(TRF_QUEUE, "rb");
    while(reading != 0) {
        reading = fread(&rq_buff, sizeof(int), 1, rq);
        if(reading != 0) {
            printf("%d\n", rq_buff);
        }
    }

    printf("\n\n---------------\n");
    return 0;
}
