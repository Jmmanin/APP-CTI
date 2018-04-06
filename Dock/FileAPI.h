/*Header file for functionalities of FileAPI
Includes calls for 
- writing files to the inner file system
- checking out work from the inner file system
- reading and viewing the inner file system
- managing the inner file system
*/
//Inclusion guard
#ifndef FILE_API_INCLUDE
#define FILE_API_INCLUDE

//Includes
#include "UniversalDefines.h"

//****************************
//Available Typing and Structs
//****************************
typedef struct trf_header {
    int stream_id;
    int sample_rate;
    double time_slice;
    int payload;
    char prev_file[LONG_FNAME_LENGTH];
    char next_file[LONG_FNAME_LENGTH];
    int link_id;
} trf_header_t;

typedef struct prdat_header {
    int stream_id;
    int sample_rate;
    double run_time;
    int payload;
    int build_state;
} prdat_header_t;

typedef struct trfb_header {
    int stream_id;
    int sample_rate;
    char first_file[LONG_FNAME_LENGTH];
    char last_file[LONG_FNAME_LENGTH];
    char readout_ptr[LONG_FNAME_LENGTH];
    int num_links;
    double run_time;
    int build_state;
} trfb_header_t;

//****************************
//Stream Creation Function
//      --Generates a new stream in the file environment
//      --Returns the id of the new stream (0 is the invalid stream identifier)
//          ---it is up to the caller to track the stream ids for the correct streams
//****************************

int create_new_rawstream(int sample_rate);

//****************************
//Data Storage Functions
//      --Directs a buffer of data to the appropriate file while updating the file environment
//      --Require the id of the target stream, pointer to the data payload and the size of the 
//          payload in bytes/chars
//      ---Returns exit code 0 if successful or n > 0 relating to a specific issue if not.
//****************************

int store_raw_chunk(int stream_id, char *buffer_ptr, int chunk_payloadsize);

int store_processed_chunk(int stream_id, char *buffer_ptr, int chunk_payloadsize);

int store_processed_whole(int stream_id, char *buffer_ptr, int whole_payloadsize);
//****************************
//Stream Capping Functions
//      --Close parts of the stream off from accepting new data writes
//      --Require the id of the target stream
//          ---Capping a processed file requires an override if its associated temporary stream files are not capped already
//      --Returns 0 if success or an exit code n > 0  describing issue
//****************************

int cap_rawstream(int stream_id);

int cap_processed_file(int stream_id, int force_cap);  //0 - enforce data stream matching, 1 - force capping process.

//****************************
//Stream Reading Functions
//      --Buffer data from files for working and processing
//          ---trf files are checked out and the checkout_ function is dual purpose
//          ---prdat files are simply buffered in their entirity from the file
//      --Require the target stream id, a payload buffer location and an appropriate header struct location
//      --Returns: as described
//****************************

        //Function will either pull next chunk from a specified stream or select the next most needy chunk from an internal queue
        //  To get work from a specific stream, specify stream_id
        //  To get work from the queue set stream_id to 0.
        //Returns the id of the stream the work buffered is coming from. 
        //  If stream_id is specified, the return value is stream_id unless there is no work available from that stream
        //  If stream_id is not specified, the return value is the whatever stream the work comes from unless the queue is empty
        //   in which case it returns 0, meaning no work is available in the FS
int checkout_raw_chunk(int stream_id, char *chunk_buff, trf_header_t *meta_buffer);

        //Function targets and buffers contents of the target stream's PRDAT file.
        //Returns 0 if success, 1 if the file does not exist
int read_processed_stream(int stream_id, prdat_header_t *meta_buffer, char *data_buffer, int buffer_cap);

//****************************
//FS Management Functions
//      --Work as described
//****************************

        //Ensures file system is in a valid state to access work
void FS_Init(); 

        //Prints out human readable version of the contents of the state files
void FS_View_StateFiles();

        //creates and stores an image of the db to a different folder
int db_backup();

        //Overwrites files in the system with those in the image folder
int db_restore_db();

#endif
