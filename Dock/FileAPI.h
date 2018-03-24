/*Header file for functionalities of FileAPI
Includes calls for 
- writing files to the inner file system
- checking out work from the inner file system
- reading and viewing the inner file system
- managing the inner file system
*/

//TODO these need to be made into macros! Not just defines so they are available as part of the module
#define MAX_STREAM_TIME  300  /*length of time in seconds a single stream will be constrained to*/
#define MAX_SAMPLE_RATE 60 /*60hz max sample rate*/
#define SAMPLE_SIZE 1 /*standardized sample size*/
#define MAX_BUFF_SIZE (MAX_STREAM_TIME * MAX_SAMPLE_RATE * SAMPLE_SIZE) /*largest possible buffer size*/

#define SHORT_FNAME_LENGTH 20  /*The lengths of buffers commonly used for storing filename strings*/
#define STD_FNAME_LENGTH 30
#define LONG_FNAME_LENGTH 50

//****************************
//Available Typing and Structs
//****************************
typedef struct trf_header {
    int stream_id;
    int sample_rate;
    int time_slice;
    int payload;
    char prev_file[LONG_FNAME_LENGTH];
    char next_file[LONG_FNAME_LENGTH];
    int link_id;
} trf_header_t;

typedef struct prdat_header {
    int stream_id;
    int sample_rate;
    int run_time;
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
    int run_time;
    int build_state;
} trfb_header_t;
//****************************
// file writing Functions
//****************************
//		RAW DATA STREAM
int create_new_rawstream(int sample_rate);

int store_raw_chunk(int stream_id, char *buffer_ptr, int chunk_timelength);

int cap_rawstream(int stream_ptr);

//		PROCESSED FILE
int store_processed_chunk(int stream_id, char *buffer_ptr, int chunk_timelength);

int store_processed_whole(int stream_id, char *buffer_ptr, int whole_timelength);

int cap_processed_file(int stream_id, int force_cap);

//****************************
// Work checkout Functions - depreciated
//***************************

int checkout_raw_chunk(int stream_id, char *chunk_buff, trf_header_t *meta_buffer);

//****************************
//Read/Traverse Functions
//****************************

int read_raw_chunk(int stream_id, trf_header_t *meta_buffer, char *data_buffer, int buffer_cap);

int read_processed_stream(int stream_id, prdat_header_t *meta_buffer, char *data_buffer, int buffer_cap);

//****************************
//FS Management Functions
//****************************
void FS_Init(); 

int *db_view_db();

int db_backup();

int db_restore_db();

int db_restore_file();
