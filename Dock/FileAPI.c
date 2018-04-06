//The API function set of calls for accessing and managing the database schema
//Brendan Luksik and Jeremy Manin
//Senior Design (COE 1896) 2018
//bk12@pitt.edu

//TODO: -------CONSIDER STANDARDIZING THE EXIT CODES----------- 

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
#include "FileAPI.h"
#include "UniversalDefines.h"

//structs and enums

/*state for whether stream accepts new data*/
enum build_states {
    OPEN,      /*Accepting new data chunks*/
    UPTODATE,  /*Accepting new data chunks but */
    COMPLETE   /*Not accepting new data*/
};

/*types of files existant in the system*/
enum file_types {
    TRF,
    TRFB,
    PRDAT,
    MNGR
};

//PROTOTYPES
void FS_get_file_name(char *filename_s, int stream_id, int style, int iteration);
int FS_create_stream_id();
int FS_write_rawstream_base_file(trfb_header_t meta_data);
int FS_register_stream(int stream_id);
int FS_check_file(char *filename);
int FS_check_stream(int stream_id);
int FS_pull_from_q();
int FS_add_to_q(int stream_id);
trfb_header_t FS_get_trfb(int stream_id);


/*creates new rawstream resource for callers to add buffers into
    -returns: id of the resource (int)
*/
int create_new_rawstream(int sample_rate) {
    int stream_id = FS_create_stream_id();
    if (!stream_id) { 
        return 0;  //Zero is never a valid stream_id
    }

    trfb_header_t base_header = {stream_id, sample_rate, "", "", "", 0, 0, UPTODATE};

    if (!FS_write_rawstream_base_file(base_header)) { 
        return stream_id;
    }
    return 0;

}

/*pulls a buffered set of data into a raw file stream
exit statuses: 
    - 0: all good
    - 1: stream_ptr points to a stream the does not exist
    - 2: stream_ptr points to a COMPLETE stream
*/
int store_raw_chunk(int stream_id, char *buffer_ptr, int chunk_numPkts) {
    trfb_header_t stream_info;
    trf_header_t new_header;
    trf_header_t prevtrail_header;
    
    int i;
    int first_commit_flag = 0;
    
    char trf_file_s[LONG_FNAME_LENGTH];
    char trfb_s[STD_FNAME_LENGTH];
    char prevtrail_payload[MAX_TRFBUFF_SIZE];
    
    FILE *trfb, *trailing_trf, *new_entry;

    //ENSURE VALID FILESTREAM
    FS_get_file_name(trfb_s, stream_id, TRFB, 0);
    if(!FS_check_file(trfb_s)) {
        return 1; //exit status for stream DNE
    }
    
        //get base file
    trfb = fopen(trfb_s, "rb");
    fread(&stream_info, sizeof(trfb_header_t), 1, trfb);
    fclose(trfb);

    if(stream_info.build_state == COMPLETE) {
        return 2;  //stream is not accepting chunks
    }
    if(stream_info.num_links == 0) {
        first_commit_flag = 1;
    }

        //update new header info
    FS_get_file_name(trf_file_s, stream_id, TRF, (stream_info.num_links));
    new_header.link_id = stream_info.num_links;
    new_header.stream_id = stream_info.stream_id;
    new_header.sample_rate = stream_info.sample_rate;
    new_header.time_slice = stream_info.run_time;
    new_header.payload = chunk_numPkts * INP_PKT_SIZE;
    for(i = 0; i < LONG_FNAME_LENGTH; i++) {
        new_header.prev_file[i] = stream_info.last_file[i];
    }

        //update base header info
    if(first_commit_flag == 1) {
        for(i = 0; i < LONG_FNAME_LENGTH; i++) {
            stream_info.first_file[i] = trf_file_s[i];
            stream_info.readout_ptr[i] = trf_file_s[i];
        }
    } 
    if(stream_info.build_state == UPTODATE) {
        for(i = 0; i < LONG_FNAME_LENGTH; i++) {
            stream_info.readout_ptr[i] = trf_file_s[i];
        }
        stream_info.build_state = OPEN;
        FS_add_to_q(stream_id);
    }
    for(i = 0; i < LONG_FNAME_LENGTH; i++) {
        stream_info.last_file[i] = trf_file_s[i];
    }    
    stream_info.num_links += 1;
    stream_info.run_time += stream_info.sample_rate * chunk_numPkts; 
    
        //update trailing file info
    if(!first_commit_flag) {
            //pull in file
        trailing_trf = fopen(new_header.prev_file, "rb");
        fread(&prevtrail_header, sizeof(trf_header_t), 1, trailing_trf);
        fread(&prevtrail_payload, sizeof(char), prevtrail_header.payload, trailing_trf);
        fclose(trailing_trf);

            //update data
        for(i = 0; i < LONG_FNAME_LENGTH; i++) {
            prevtrail_header.next_file[i] = trf_file_s[i];
        } 
            //write back to file
        trailing_trf = fopen(new_header.prev_file, "wb");
        fwrite(&prevtrail_header, sizeof(trf_header_t), 1, trailing_trf);
        fwrite(&prevtrail_payload, sizeof(char), prevtrail_header.payload, trailing_trf);
        fclose; 
    }  

    if(stream_info.run_time >= MAX_STREAM_TIME) {
        stream_info.build_state = COMPLETE;
    }

    //Writes back out to files
        //BASE FILE
    trfb = fopen(trfb_s, "wb");
    fwrite(&stream_info, sizeof(trfb_header_t), 1, trfb);
    fclose(trfb);
    
        //NEW ENTRY
    new_entry = fopen(trf_file_s, "wb");
    //printf("\n**fs writeout: '%s', length: %d\n", buffer_ptr, new_header.payload);
    fwrite(&new_header, sizeof(trf_header_t), 1, new_entry);
    fwrite(buffer_ptr, sizeof(char), new_header.payload, new_entry);
    fclose(new_entry);

    return 0;  //success
}

/*Stores away processed chunk, handle update of C-Table*/
int store_processed_chunk(int stream_id, char *buffer_ptr, int chunk_payloadsize) {
    int new_file_flag = 0;
    int old_payload_size = 0;
    char prdat_s[LONG_FNAME_LENGTH];
    char old_payload[MAX_PRDATBUFF_SIZE];
    FILE *prdat;
    trfb_header_t stream_info;
    prdat_header_t proc_header;

    FS_get_file_name(prdat_s, stream_id, PRDAT, 0);
    
    //check for file existence
    if(!FS_check_file(prdat_s)) {
        new_file_flag = 1;
    }

    if(new_file_flag == 1) {
        //create header
        proc_header.stream_id = stream_id;
        stream_info = FS_get_trfb(stream_id);
        proc_header.sample_rate = stream_info.sample_rate;
        proc_header.run_time = 0;
        proc_header.payload = 0;
        proc_header.build_state = OPEN;

    } else {
        //load header + old data
        prdat = fopen(prdat_s, "rb");
        fread(&proc_header, sizeof(prdat_header_t), 1, prdat);
        fread(&old_payload, sizeof(char), proc_header.payload, prdat);
        fclose(prdat);

    }
    old_payload_size = proc_header.payload;
    proc_header.payload += chunk_payloadsize; /*one of these will have to be scaled!!!*/
    proc_header.run_time += chunk_payloadsize / (TRNS_PKT_SIZE * proc_header.sample_rate);

    prdat = fopen(prdat_s, "wb");
    fwrite(&proc_header, sizeof(prdat_header_t), 1, prdat);
    fwrite(&old_payload, sizeof(char), old_payload_size, prdat);
    fwrite(buffer_ptr, sizeof(char), chunk_payloadsize, prdat);
    fclose(prdat);

    return 0;
}

/*Store away all processed chunks at once, handle update of C-Table*/
int store_processed_whole(int stream_id, char *buffer_ptr, int whole_payloadsize) {
    int store_stat, cap_stat;
    store_stat = store_processed_chunk(stream_id, buffer_ptr, whole_payloadsize);
    cap_stat = cap_processed_file(stream_id, 0);

    if(!store_stat && !cap_stat) {
        return 0;   //success
    } else if(store_stat) {
        return 1;   //issue in writing out the file
    } else {
        return 2;   //issue in capping the file
    }

}

/*caps the processed file:
  - 0: file has been set to COMPLETE state, it does not accept more data
  - 1: a non existent stream is targeted. Nothing happened.
*/ 
int cap_rawstream(int stream_id) {
    char trfb_s[LONG_FNAME_LENGTH];
    FS_get_file_name(trfb_s, stream_id, TRFB, 0);
    if(!FS_check_file(trfb_s)) {
        return 1;
    }

    trfb_header_t stream_info = FS_get_trfb(stream_id);
    stream_info.build_state = COMPLETE;
    FS_write_rawstream_base_file(stream_info);
    return 0;

}

/*caps the processed file:
  - 0: file has been set to COMPLETE state, it does not accept more data
  - 1: amount of prdat does not match the amount of trf data, file not capped
  - 2: amount of prdat does not mathc the amount of trf data but override is on, file is capped
*/
int cap_processed_file(int stream_id, int force_cap) {
    FILE *prdat;
    char prdat_s[LONG_FNAME_LENGTH];
    char prdat_buff[MAX_PRDATBUFF_SIZE];
    prdat_header_t proc_header;
    trfb_header_t stream_header = FS_get_trfb(stream_id);

    if((stream_header.build_state == COMPLETE) && !force_cap) {
        return 1;
    }

    FS_get_file_name(prdat_s, stream_id, PRDAT, 0);

    prdat = fopen(prdat_s, "rb");  //NOTE: Opening and closing files could be their own functions
    fread(&proc_header, sizeof(prdat_header_t), 1, prdat);
    fread(&prdat_buff, sizeof(char), proc_header.payload, prdat);
    fclose(prdat);

    proc_header.build_state = COMPLETE;
    
    prdat = fopen(prdat_s, "wb");
    fwrite(&proc_header, sizeof(prdat_header_t), 1, prdat);
    fwrite(&prdat_buff, sizeof(char), proc_header.payload, prdat);
    fclose(prdat);
    
    if(stream_header.build_state == COMPLETE) {
        return 2;
    }
    return 0;
}

/*reads out a file to a buffer, and properly checks out the file from the work system
stream_id input:
    0 - gets a filestream from the queue and its next work buffer
    n - get the next work buffer from the specified queue
return:
    0 - no data to read out
    n > 0 - stream_id work buffer belongs to. 
*/
int checkout_raw_chunk(int stream_id, char *chunk_buff, trf_header_t *meta_buffer) {
    printf("** Stream_id: %d: \n",stream_id);
    int i;
    trf_header_t target_meta;
    trfb_header_t target_base_meta;
    char target_buff[MAX_TRFBUFF_SIZE];
    char trfb_s[LONG_FNAME_LENGTH];

    //setup target stream
    if(stream_id == 0) {
        stream_id = FS_pull_from_q();
        if(stream_id == 0) {
            return 0; //there is nothing in the queue to work on
        }
    }

    target_base_meta = FS_get_trfb(stream_id);
    if(strcmp(target_base_meta.readout_ptr, "") == 0) {
        printf("** exiting bc read ptr == ''\n");
        return 0;  //there is nothing in the stream to work on. 
    }


    //readout target file
    FILE *target_trf = fopen(target_base_meta.readout_ptr, "rb");
    fread(meta_buffer, sizeof(trf_header_t), 1, target_trf);
    fread(chunk_buff, sizeof(char), target_meta.payload, target_trf);
    fclose(target_trf);

    //update pointers
    for(i = 0; i < LONG_FNAME_LENGTH; i++) {
        target_base_meta.readout_ptr[i] = meta_buffer->next_file[i];
    }
    if(strcmp(target_base_meta.readout_ptr, "") == 0) {
        if(target_base_meta.build_state != COMPLETE) {
            target_base_meta.build_state = UPTODATE;
        }
    }
    
    printf("**next read: %s\n", target_base_meta.readout_ptr);
    FS_get_file_name(trfb_s, stream_id, TRFB, 0);
    FILE *trfb = fopen(trfb_s, "wb");
    fwrite(&target_base_meta, sizeof(trfb_header_t), 1, trfb);
    fclose(trfb);

    //close out function
    //chunk_buff = target_buff;
    //meta_buffer = &target_meta;
    return stream_id;

}

int read_processed_stream(int stream_id, prdat_header_t *meta_buffer, char *data_buffer, int buffer_cap) {
    
    char prdat_s[LONG_FNAME_LENGTH];
    FS_get_file_name(prdat_s, stream_id, PRDAT, 0);
    if(!FS_check_file(prdat_s)) {
        return 1;
    }

    FILE* target_trf = fopen(prdat_s, "rb");
    fread(meta_buffer, sizeof(prdat_header_t), 1, target_trf);

    if(buffer_cap < MAX_PRDATBUFF_SIZE) {
        //Protects from segfaults but my not read out whole target
        fread(data_buffer, sizeof(char), buffer_cap, target_trf);
    } else {
        fread(data_buffer, sizeof(char), MAX_PRDATBUFF_SIZE, target_trf);
    }
    fclose(target_trf);
    
    return 0;
}

int db_backup() {} /*lol need to implement these eventually*/
int db_restore_db() {}
int db_restore_file() {}

//Internal functionalities
int FS_create_stream_id() {
    //id selection function here
    int id_state;

    FILE *curr_state = fopen(STATE_TABLE, "rb");
    if(curr_state == NULL) { 
        return 0; }

    fread(&id_state, sizeof(int), 1, curr_state);
    id_state += 1;
    fclose(curr_state);

    curr_state = fopen(STATE_TABLE, "wb");
    if(curr_state == NULL) { 
        return 0; }
    fwrite(&id_state, sizeof(int), 1, curr_state);

    fclose(curr_state);
    return id_state;
}
int FS_write_rawstream_base_file(trfb_header_t file_info) {

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
            sprintf(filename_s, "storage/%d_%d.trf", stream_id, iteration);
            break;
        case TRFB:
      	//follows the pattern of: <stream_id>.trf
            sprintf(filename_s, "storage/%d.trf", stream_id);
            break;
        case PRDAT:
      	//follows the pattern of: <stream_id>.prdat
            sprintf(filename_s, "storage/%d.prdat", stream_id);
            break;
    }
    return;
}
/*returns 0 if add, 1 if queue issue*/
int FS_add_to_q(int stream_id) {
    int q_length;
    int num_jobs;
    int *qbuf;
    
    if(!FS_check_file(TRF_QUEUE)) {
        return 1;
    }
    if(stream_id == 0) {
        return 1;
    }

    FILE *work_q = fopen(TRF_QUEUE, "rb");
    
    fseek(work_q, 0, SEEK_END);
    q_length = ftell(work_q);
    fseek(work_q, 0, SEEK_SET);
    num_jobs = q_length / sizeof(int);

    qbuf = (int *) malloc(q_length + sizeof(int));
    fread(qbuf, sizeof(int), num_jobs, work_q);
    fclose(work_q);
    
    qbuf[num_jobs] = stream_id;
    num_jobs += 1;

    work_q = fopen(TRF_QUEUE, "wb");
    fwrite(qbuf, sizeof(int), num_jobs, work_q);

    fclose(work_q);
    free(qbuf);
    return 0;
}

int FS_pull_from_q() {
    int q_length;
    int num_jobs;
    int next_job;
    int *qbuf;
    
    if(!FS_check_file(TRF_QUEUE)) {
        return 0;
    }

    FILE *work_q = fopen(TRF_QUEUE, "rb");
    
    fseek(work_q, 0, SEEK_END);
    q_length = ftell(work_q);
    if(q_length == 0 || q_length < sizeof(int)) {
        fclose(work_q);
        return 0;
    }

    fseek(work_q, 0, SEEK_SET);
    num_jobs = q_length / sizeof(int);

    qbuf = (int *) malloc(q_length);
    fread(qbuf, sizeof(int), num_jobs, work_q);
    fclose(work_q);
    
    next_job = qbuf[0];
    
    work_q = fopen(TRF_QUEUE, "wb");
    fwrite(&qbuf[1], sizeof(int), (num_jobs-1), work_q);

    return next_job;
}
//MANAGEMENT TABLE CALLS
    //FILE SYSTEM LOOKUP TABLE (fs-table) BOOLEAN
int FS_register_stream(int stream_id) {
    char stream_id_s[SHORT_FNAME_LENGTH];
    char trfb_s[LONG_FNAME_LENGTH];
    FS_get_file_name(trfb_s, stream_id, TRFB, 0);
    //if stream is in table throw 1 exit
    if (!FS_check_file(trfb_s)) {
   	//printf("***FS: file already exists: not registering.");
        return 0;
    } 
    else {
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

trfb_header_t FS_get_trfb(int stream_id) {
    char target_file[LONG_FNAME_LENGTH];
    FS_get_file_name(target_file, stream_id, TRFB, 0);
    trfb_header_t target_header;

    FILE *target_trfb = fopen(target_file, "rb");
    fread(&target_header, sizeof(trfb_header_t), 1, target_trfb);
    fclose(target_trfb);

    return target_header;
}

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

    //check for work queue
    if(!FS_check_file(TRF_QUEUE)) {
        FILE *trf_queue = fopen(TRF_QUEUE, "wb");
        fclose(trf_queue);
    }
}

void FS_View_StateFiles() {
    FS_Init();

    FILE *st, *fs, *rq;
    int st_buff;
    char fs_buff[1024];
    int rq_buff;
    int reading = 1;
    
    //state table
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

    printf("\n\nAll files printed.\n");
}

/*replaced with checkout_raw_chunk
int read_raw_chunk(int stream_id, trf_header_t *meta_buffer, char *data_buffer, int buffer_cap) {
    char trfb_s[LONG_FNAME_LENGTH];
    FS_get_file_name(trfb_s, stream_id, TRFB, 0);
    if(!FS_check_file(trfb_s)) {
        return 2;
    }
    //TODO: ^^kind of redundant to open and close all these files twice
    trfb_header_t director = FS_get_trfb(stream_id);
    
    if(strcmp(director.readout_ptr, "") == 0) {
        return 1; //There is no data to read in
    }

    FILE* target_trf = fopen(director.readout_ptr, "rb");
    fread(meta_buffer, sizeof(trf_header_t), 1, target_trf);

    if(buffer_cap < MAX_BUFF_SIZE) {
        //Protects from segfaults but my not read out whole target
        fread(data_buffer, sizeof(char), buffer_cap, target_trf);
    } else {
        fread(data_buffer, sizeof(char), MAX_BUFF_SIZE, target_trf);
    }
    fclose(target_trf);
    
    return 0;
}*/
