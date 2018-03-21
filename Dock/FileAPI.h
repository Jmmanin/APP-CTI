/*Header file for functionalities of FileAPI
Includes calls for 
- writing files to the inner file system
- checking out work from the inner file system
- reading and viewing the inner file system
- managing the inner file system
*/
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
    int num_links;
    int run_time;
    int build_state;
} trfb_header_t;
//****************************
// file writing Functions
//****************************
//		RAW DATA STREAM
int create_new_rawstream(int sample_rate);

int store_raw_chunk(int stream_id, char *buffer_ptr, int sample_rate);

int cap_rawstream(int stream_ptr);

//		PROCESSED FILE
int store_processed_chunk(int stream_id, char *buffer_ptr, int chunk_timelength);

int store_processed_whole(int stream_id, char *buffer_ptr, int whole_timelength);

int cap_processed_file(int stream_id, int force_cap);

//****************************
// Work checkout Functions
//****************************
int checkout_raw_chunk();

int checkout_raw_chunk(void *stream_ptr);

//****************************
//Read/Traverse Functions
//****************************

int read_raw_chunk(void *stream_ptr);

int read_processed_stream(void *stream_ptr);

//****************************
//FS Management Functions
//****************************
void FS_Init(); 

int *db_view_db();

int db_backup();

int db_restore_db();

int db_restore_file();
