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

#define SHORT_FNAME_LENGTH 20  /*The lengths of bufers commonly used for storing filename strings*/
#define STD_FNAME_LENGTH 30
#define LONG_FNAME_LENGTH 50

#define MAX_STREAM_TIME  300  /*length of time in seconds a single stream will be constrained to*/

//structs and enums
typedef struct trf_header {
	int stream_id;
	int sample_rate;
	int time_slice;
	char prev_file[LONG_FNAME_LENGTH];
	char next_file[LONG_FNAME_LENGTH];
} trf_header_t;

typedef struct prdat_header {
	int stream_id;
	int sample_rate;
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
int create_rawstream_base_file(trfb_header_t meta_data);
void FS_get_file_name(char *filename_s, int stream_id, int style, int iteration);
int FS_register_stream(int stream_id);
int FS_check_file(char *filename);
int FS_check_stream(int stream_id);


/*creates new rawstream resource for callers to add buffers into
	-returns: id of the resource (int)
*/
int create_new_rawstream(int sample_rate) {
	int stream_id = create_stream_id();
	if (!stream_id) { return 0; }

	trfb_header_t base_header = {stream_id, sample_rate, NULL, NULL, 0, 0, NEW};

	if (!create_rawstream_base_file(base_header)) { return stream_id; }
	return 0;

}


/*creates new processed data file for */
int create_new_processed_file(int rawstream_id) {
	/*prdat_header_t proc_header = {rawstream_id, 0, OPEN};
	FILE *new_proc_file = fopen(FS_get_file_name(rawstream_id, PRDAT, 0), "wb");

	if (fwrite(&proc_header, sizeof(prdat_header_t), 1, new_proc_file) != 1) {
		return 1;
	}*/

	return 0;
}

/*pulls a buffered set of data into a raw file stream
exit statuses: 
	- 0: all good
	- 1: stream_ptr points to a stream the does not exist
	- 2: Could not find or access the previous trailing file
*/
int store_raw_chunk(int stream_ptr, int *buffer_ptr, int chunk_timelength) {
	trfb_header_t stream_info;
	trf_header_t new_header;
	trf_header_t prevtrail_header;
	int i, payload_length;
	char trf_file_s[LONG_FNAME_LENGTH];
	char trfb_s[STD_FNAME_LENGTH];

	//if(FS_check_stream() != open trf) { do not store chunk }
	FS_get_file_name(trfb_s, stream_ptr, TRFB, 0);
	if(!FS_check_file(trfb_s)) {
		return 1; //exit status for stream DNE
	}

	FILE *trfb = fopen(trfb_s, "rb");
	fread(&stream_info, sizeof(trfb_header_t), 1, trfb);
	fclose(trfb);

	//update new header info
	FS_get_file_name(trf_file_s, stream_ptr, TRF, stream_info.num_links);
	new_header.sample_rate = stream_info.sample_rate;
	new_header.time_slice = stream_info.run_time;
	payload_length = stream_info.sample_rate * chunk_timelength;
	for(i = 0; i < LONG_FNAME_LENGTH; i++) {
		new_header.prev_file[i] = stream_info.last_file[i];
	}

	//pull and update last file in the double link list
	FILE *trailing_trf = fopen(new_header.prev_file, "rb");
	if(trailing_trf == NULL) {
		if(stream_info.num_links != 0) {
			return 2; //corrupted or missing link in chain
		}
	} else {
		fread(&prevtrail_header, sizeof(trf_header_t), 1, trailing_trf);
		fclose(trailing_trf);
		for(i = 0; i < LONG_FNAME_LENGTH; i++) {
			prevtrail_header.next_file[i] = trf_file_s[i];
		}
	}

	//update base header info
	stream_info.num_links += 1;
	stream_info.run_time += chunk_timelength;
	for(i = 0; i < LONG_FNAME_LENGTH; i++) {
		stream_info.last_file[i] = trf_file_s[i];
	}

	//Writes back out to files
	//trfb file
	trfb = fopen(trfb_s, "wb");  //ok to overwrite whole file
	trailing_trf = fopen(new_header.prev_file, "wb"); //How modify file correctly?
	FILE *new_trf = fopen(trf_file_s, "wb");

	fwrite(&stream_info, sizeof(trfb_header_t), 1, trfb);
	//put trailing fwrite here - must overwrite header but not payload - 
	fwrite(&new_header, sizeof(trf_header_t), 1, new_trf);
	fwrite(buffer_ptr, sizeof(int), payload_length, new_trf);

	fclose(trfb);
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
int create_rawstream_base_file(trfb_header_t file_info) {
	
	char filename_string[STD_FNAME_LENGTH];
	FS_get_file_name(filename_string, file_info.stream_id, TRFB, 0);
	int DBGC_reg_stat;
	//create the base file with the header
	FILE *new_trfb = fopen(filename_string, "wb");
	if (new_trfb == NULL) {
		return 1;
	}

	if (fwrite(&file_info, sizeof(trfb_header_t), 1, new_trfb) != 1) {
		return 2;
	}

	DBGC_reg_stat = FS_register_stream(file_info.stream_id);
	fclose(new_trfb);
	return 0;
}

void FS_get_file_name(char *filename_s, int stream_id, int style, int iteration) {

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
	//FILE SYSTEM LOOKUP TABLE (fs-table) BOOLEAN
int FS_register_stream(int stream_id) {
	char stream_id_s[SHORT_FNAME_LENGTH];
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
	//looks for a file in the folder BOOLEAN
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