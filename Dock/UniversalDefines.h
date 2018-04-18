//All defines needed by the server in a centrally defined place

//inclusion guard
#ifndef DEFINES_INCLUDE
#define DEFINES_INCLUDE

#define INP_PKT_SIZE 52            /*number of bytes/packet incoming from the rig*/
#define TRNS_PKT_SIZE 52           /*number of bytes/packet after being run through transform function*/
#define MAX_STREAM_TIME  300       /*length of time in seconds a single stream will be constrained to*/
#define MAX_TRF_SEGMENT 60         /*length of time the INP buffer will run for*/
#define MAX_SAMPLE_RATE 60         /*60hz max sample rate*/
#define STD_TRANSP_TIME 10         /*number of seconds worth of data buffered by dock before shipping off to file system*/
#define DOCK_MISS_TRIGGER 100        /*total time of loop misses before we declare a rig offline*/

#define SVR_MONITORING_DELAY 0.25 /*how long the master thread in the server will pause between cycles*/

#define Q_START_SIZE 256           /*start value of q_length (in packets)*/

#define FS_TABLE "storage/fs_fileSystem.txt"
#define STATE_TABLE "storage/fs_state.bin"
#define TRF_QUEUE "storage/fs_rawQueue.bin"
#define CHECKOUT_TABLE "storage/fs_coTable.bin"

#define SHORT_FNAME_LENGTH 20  /*The lengths of buffers commonly used for storing filename strings*/
#define STD_FNAME_LENGTH 30
#define LONG_FNAME_LENGTH 50

#define MAX_TRFBUFF_SIZE (MAX_TRF_SEGMENT * MAX_SAMPLE_RATE * INP_PKT_SIZE)  /*largest possible buffer sizes*/
#define MAX_PRDATBUFF_SIZE (MAX_STREAM_TIME * MAX_SAMPLE_RATE * TRNS_PKT_SIZE) 

#define NUM_WORKERS 4
#define MSG_SIZE 16
#define DOCK_W comm_block[0]
#define TRNS_W comm_block[1]
#define DPS_W comm_block[2]
#define INP_W comm_block[3]
#define LIVS_W comm_block[4]

#endif
