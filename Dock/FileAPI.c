//The API function set of calls for accessing and managing the database schema
//Brendan Luksik and Jeremy Manin
//Senior Design (COE 1896) 2018
//bk12@pitt.edu

//TODO: what type of pointer should the file and stream pointers be?

/*OVERVIEW OF ARCHITECTURE
	
	RAW STREAMS
		- A Raw Stream is created and can hold an amount of data up to the configured maximum (which can be turned off)
		- Chunks of buffered data can be spliced into the stream as they become available
	PROCESSED FILES:
		- Processed Files are the final dataset streams for GUI and algorithmic use
		- Once a file is created it can be both accessed as an available processed stream
		- 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//defines and global state declares
#define FS_TABLE "fs_fileSystem.txt"
#define STATE_TABLE "state.bin"
#define TRF_QUEUE "fs_rawQueue.bin"
#define CHECKOUT_TABLE "fs_coTable.bin"
#define MAX_GAP_CAP 10        /*number of gaps supported in this configuration of the system*/
#define MAX_STREAM_TIME  300  /*length of time in seconds a single stream will be constrained to*/

//structs and enums
typedef struct trf_header {
	int stream_id;
	int sample_rate;
	int time_slice;
	void *prev_file;
	void *next_file;
} trf_header_t;

typedef struct prdat_header {
	int stream_id;
	int sample_rate;
	int build_state;
} prdat_header_t;

typedef struct trfb_header {
	int stream_id;
	int sample_rate;
	void *first_file;
	void *last_file;
	int num_links;
	int run_time;
	int build_state;
} trfb_data_t;

/*state for whether stream accepts new data*/
enum build_states {
	OPEN,
	COMPLETE
};

/*current state of completeness for a stream*/
enum stream_states {
	NEW,
	RAW,
	PARTIAL,
	DONE
};

enum constraint_types {
	NONE,
	TIME,
	BYTES
};

/*types of files existant in the system*/
enum file_types {
	TRF,
	TRFB,
	PRDAT,
	MNGR
};

//PROTOTYPES
int create_stream_id();
void create_file_name(char *filename_s, int stream_id, int style, int iteration);
int create_rawstream_base_file(trf_header_t meta_data, trfb_data_t data);
int FS_register_stream(int stream_id);
int FS_check_file(char *filename);
int FS_check_stream(int stream_id);


/*creates new rawstream resource for callers to add buffers into
	-returns: id of the resource (int)
*/
int create_new_rawstream() {
	int stream_id = create_stream_id();
	if (!stream_id) { return 0; }

	trf_header_t base_header = {stream_id, 0, -1, NULL, NULL};
	trfb_data_t base_data = {0, 0, OPEN};

	if (!create_rawstream_base_file(base_header, base_data)) { return stream_id; }
	return 0;
}

/*creates new processed data file for */
int create_new_processed_file(int rawstream_id) {
	/*prdat_header_t proc_header = {rawstream_id, 0, OPEN};
	FILE *new_proc_file = fopen(create_file_name(rawstream_id, PRDAT, 0), "wb");

	if (fwrite(&proc_header, sizeof(prdat_header_t), 1, new_proc_file) != 1) {
		return 1;
	}*/

	return 0;
}

/*pulls a buffered set of data into a raw file stream*/
int store_raw_chunk(int stream_ptr, int *buffer_ptr, int sample_rate) {

}

/*Stores away processed chunk, handle update of C-Table*/
int store_processed_chunk(void *stream_ptr, int *buffer_ptr) {}

/*Store away all processed chunks at once, handle update of C-Table*/
int store_processed_whole(void *stream_ptr, int *buffer_ptr, int buff_length) {}

int store_processed_new(prdat_header_t meta_data, int *buffer_ptr, int buff_length) {}
 
int cap_rawstream(void *stream_ptr) {}
int cap_processed_file(void *stream_ptr) {}

int read_raw_chunk(void *stream_ptr) {}
int *read_processed_stream(void *stream_ptr) {}

int db_backup() {}
int db_restore_db() {}
int db_restore_file() {}

//Internal functionalities
int create_stream_id() {
	//id selection function here
	int id_state;
	
	FILE *curr_state = fopen(STATE_TABLE, "rb");
	if(curr_state == NULL) { return 0; }
	
	fread(&id_state, sizeof(int), 1, curr_state);
	id_state += 1;
	fclose(curr_state);
	
	curr_state = fopen(STATE_TABLE, "wb");
	if(curr_state == NULL) { return 0; }
	fwrite(&id_state, sizeof(int), 1, curr_state);

	fclose(curr_state);
	return id_state;
}
int create_rawstream_base_file(trf_header_t file_info, trfb_data_t file_data) {
	
	char filename_string[50];
	create_file_name(filename_string, file_info.stream_id, TRFB, 0);
	int DBGC_reg_stat;
	//create the base file with the header
	FILE *new_trfb = fopen(filename_string, "wb");
	if (new_trfb == NULL) {
		return 1;
	}

	if (fwrite(&file_info, sizeof(trf_header_t), 1, new_trfb) != 1) {
		return 2;
	}
	if (fwrite(&file_data, sizeof(trfb_data_t), 1, new_trfb) != 1) {
		return 2;
	}

	DBGC_reg_stat = FS_register_stream(file_info.stream_id);
	fclose(new_trfb);
	return 0;
}

void create_file_name(char *filename_s, int stream_id, int style, int iteration) {

	if (stream_id == 0) {
		return;
	}

	switch (style) {
		case TRF:
			//follows the pattern of: <stream_id>_<iteration>.trf
			sprintf(filename_s, "%d_%d.trf", stream_id, iteration);
			break;
		case TRFB:
			//follows the pattern of: <stream_id>.trf
			sprintf(filename_s, "%d.trf", stream_id);
			break;
		case PRDAT:
			//follows the pattern of: <stream_id>.prdat
			sprintf(filename_s, "%d.prdat", stream_id);
			break;
	}
	return;
}

//MANAGEMENT TABLE CALLS
	//FILE SYSTEM LOOKUP TABLE (fs-table)
int FS_register_stream(int stream_id) {
	char stream_id_s[20];
	//if stream is in table throw 1 exit
	if (FS_check_stream(stream_id)) {
		//printf("***FS: file already exists: not registering.");
		return 0;
	
	} else {
		sprintf(stream_id_s, "%d\n", stream_id);

		FILE *fs_table = fopen(FS_TABLE, "a");
		//fseek(fs_table, 1, SEEK_END);
		fwrite(&stream_id_s, sizeof(char), strlen(stream_id_s), fs_table);
		fclose(fs_table);
	}
	return 1;
}

	//TODO: determine who file will be organized and seeked.
int FS_check_file(char* filename) {
	FILE *target = fopen(filename, "rb");
	
	if(target != NULL) {
		fclose(target);
		return 1;
	}
	return 0;
}

/*returns the state of the stream ID
 ToDo: properly implement state check
*/
int FS_check_stream(int stream_id) { return 0; }

//Ensure neccessary files are present for work, creates if not
void FS_Init() {
	//intial values
	int state_init = 1;

	//check for state table
	if(!FS_check_file(STATE_TABLE)) {
		FILE *state_table = fopen(STATE_TABLE, "wb");
		fwrite(&state_init, sizeof(int), 1, state_table);
		fclose(state_table);
	}

	//check for File System table
	if(!FS_check_file(FS_TABLE)) {
		FILE *fs_table = fopen(FS_TABLE, "a"); //creates empty file
		fclose(fs_table);
	}
}