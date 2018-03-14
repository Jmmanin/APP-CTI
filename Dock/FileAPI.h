/*Header file for functionalities of FileAPI
Includes calls for 
- writing files to the inner file system
- checking out work from the inner file system
- reading and viewing the inner file system
- managing the inner file system
*/


//****************************
//Available Typing and Structs
//****************************

//****************************
// file writing Functions
//****************************
//		RAW DATA STREAM
int create_new_rawstream();

int store_raw_chunk(void *stream_ptr, int *buffer_ptr, int sample_rate);

int cap_rawstream(void *stream_ptr);

//		PROCESSED FILE
int create_new_processed_file(int rawstream_id);

int store_processed_chunk(void *stream_ptr, int *buffer_ptr);

int store_processed_whole(void *stream_ptr, int *buffer_ptr, int buffer_length);

int cap_processed_file(void *stream_ptr);

//****************************
// Work checkout Functions
//****************************
int checkout_raw_chunk();

int checkout_raw_chunk(void *stream_ptr);

//****************************
//Read/Traverse Functions
//****************************

int *read_raw_chunk(void *stream_ptr);

int *read_processed_stream(void *stream_ptr);

//****************************
//FS Management Functions
//****************************
int *db_view_db();

int db_backup();

int db_restore_db();

int db_restore_file();
