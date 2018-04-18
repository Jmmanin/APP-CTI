//The unit test driver for testing the functionalities of the file read, write system

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "FileAPI.h"
#include "UniversalDefines.h"

#define NUM_TESTS 1  /*Tests in the battery*/

void print_base_buff(int base);
void print_trf_buff(trf_header_t trgt);

//---------FUNCTIONALITY TESTS-----------
	//-I test the proper creation of new filestreams
int filestream_create_test() {
	FS_Init();
	printf("create new rawstream test:\n");
	printf("should output a new .trf file and matching id for each call\n\n");

	int id_1 = create_new_rawstream(1);
	int id_2 = create_new_rawstream(1);
	printf("Stream 1: ");
	if (id_1) {
		printf("PASS: ID: %d\n", id_1);
	}
	else {
		printf("FAILED\n");
	}
	printf("Stream 2: ");
	if (id_2) {
		printf("PASS: ID: %d\n", id_2);
	}
	else {
		printf("FAILED\n");
	}

	if (id_1 && id_2) { return 1; }
	return 0;
}

int store_raw_chunk_test()
{
	char hex_buf[3] = {0x55, 0x1A, 0x45};
    char *buffer1 = hex_buf;
    char *buffer2 = "IamTest2"; 
    char buffer_cmp1[10];
	char buffer_cmp2[10];
	char target_file[50];  
    trf_header_t header1;
	trf_header_t header2;
	trfb_header_t base1;
	int response1, response2;

    FS_Init();
    printf("store raw chunk test:\n");

    int id = create_new_rawstream(1); 
    printf("id: %d\n", id);
    if(id)
    {
		printf("\nInput Buffer0: %x, %x, %x\n", hex_buf[0], hex_buf[1], hex_buf[2]);
		printf("\nInput Buffer1:'%s'\n\n", buffer2);

		/*NOTE: The function expects chunk_timelength as a value in seconds, not buffer length itself*/
        response1 = store_raw_chunk(id, buffer1, 3); //Everything is set up to assume 1 letter per second
        response2 = store_raw_chunk(id, buffer2, 8);                
        
		printf("store1 exit code: %d\n", response1);
		printf("store2 exit code: %d\n\n", response2);

		sprintf(target_file, "storage/%d_0.trf", id);
		FILE *f_ptr = fopen(target_file, "rb");
        fread(&header1, sizeof(trf_header_t), 1, f_ptr);
        fread(&buffer_cmp1, sizeof(char), header1.payload, f_ptr);
        fclose(f_ptr);

		printf("_0.trf: id %d\n rate: %d\n timeslice: %lf\n payload: %d\n pfile: %s\n nfile: %s\n\n",
     		header1.stream_id, header1.sample_rate, header1.time_slice, header1.payload, header1.prev_file, header1.next_file);

		sprintf(target_file, "storage/%d_1.trf", id);
		f_ptr = fopen(target_file, "rb");
        fread(&header2, sizeof(trf_header_t), 1, f_ptr);
        fread(&buffer_cmp2, sizeof(char), header2.payload, f_ptr);
        fclose(f_ptr);

		printf("_1.trf: id %d\n rate: %d\n timeslice: %lf\n payload: %d\n pfile: %s\n nfile: %s\n\n",
     		header2.stream_id, header2.sample_rate, header2.time_slice, header2.payload, header2.prev_file, header2.next_file);	
        
		printf("Buffer0:%x, %x, %x\n", buffer_cmp1[0], buffer_cmp1[1], buffer_cmp1[2]);
		printf("Buffer1:'%s'\n", buffer_cmp2);
    }
    
    return 0;
}

int test_cap_rawstream() {
	int resp0, resp1, resp_cap;
	int id = create_new_rawstream(1);  //create new rawstream with sample rate 2/second
	char *accept_buffer = "IamTheSenate";
	char *reject_buffer = "ExecuteOrder66";

	printf("\nTesting stream capping functionality:\n\n");
	resp0 = store_raw_chunk(id, accept_buffer, 12);
	resp_cap = cap_rawstream(id);
	resp1 = store_raw_chunk(id, reject_buffer, 14);

	printf("Response1: %d | Response2: %d\n\n", resp0, resp1);
	print_base_buff(id);

	if(resp0 == 0 && resp1 == 2) {
		printf("Test PASS\n");
		return 1;
	} else {
		printf("Test FAIL\n");
		return 0;
	}
} 

int store_procbuff_test() {
	int resp0, resp1, resp2, resp3;
	int id = create_new_rawstream(1);

	printf("\nTest writing successive processed buffers back in (see file %d.prdat\n", id);
	char *first_store = "A long time ago in a galaxy far far away, Turmoil has engulfed the \
	 galatic republic, The taxation of trade routes to outlying star systems is in despute.";
	char *second_store = "Ok, here we go. Focus. Speed.I am speed. One winner, 42 losers. I eat \
	 losers for breakfast. Breakfast. Wait should I eat breakfast? A little brecky could be good...";
	char *third_store = "Im a third buffer. What? Im out of ideas";
	char *reject_store = "You shouldn't see me.";

	store_processed_chunk(id, first_store, strlen(first_store));
	store_processed_chunk(id, second_store, strlen(second_store));
	store_processed_chunk(id, third_store, strlen(third_store));

	return 1;
}

int read_two_chunk_test() {
	trf_header_t header_out;
	trf_header_t header_out2;
	trf_header_t header_out3;
	char chunk_out[MAX_TRFBUFF_SIZE];
	char chunk_out2[MAX_TRFBUFF_SIZE];
	char chunk_out3[MAX_TRFBUFF_SIZE];
	int stream_id;

	stream_id = checkout_raw_chunk(0, chunk_out, &header_out);
	printf("Stream_id: %d: \n",stream_id);
	print_trf_buff(header_out);
	printf("data: %s\n\n", chunk_out);
	//print_base_buff(stream_id);
	printf("\n\n");

	stream_id = checkout_raw_chunk(stream_id, chunk_out2, &header_out2);
	printf("Stream_id: %d: \n",stream_id);
	//print_trf_buff(header_out2);
	printf("data: %s\n\n", chunk_out2);

	stream_id = checkout_raw_chunk(stream_id, chunk_out2, &header_out);
	printf("id after links exhausted: %d\n", stream_id);

	stream_id = checkout_raw_chunk(stream_id, chunk_out3, &header_out3);
	printf("Stream_id: %d: \n",stream_id);
	//print_trf_buff(header_out3);
	printf("data: %s\n\n", chunk_out3);
	return 0;
}

/*int timing_test() {
	clock_t start, end;
	double create_ti, storeRaw_ti, storeProc_ti, readOut_ti;
	trf_header_t meta_b;
	int id, i, resp0;
	char value = 0xFF;
	char *smallBuff = "I served my nickel you come and take me!";
	char medBuff[200];
	char largeBuff[MAX_BUFF_SIZE];
	char readinBuff[MAX_BUFF_SIZE];

	for(i = 0; i < 200; i++) {
		if(value == 0x00) {
			value = 0xFF;
		}
		medBuff[i] = value;
		value -= 1;
	}

	for(i = 0;  i < MAX_BUFF_SIZE; i++) {
		if(value == 0x00) {
			value = 0xFF;
		}
		largeBuff[i] = value;
		value -= 1;
	}

	printf("\nFunctionality time test:\n");
	FS_Init();

	start = clock();
	id = create_new_rawstream(1);
	end = clock();
	create_ti = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("\ncreate_new_rawstream() time: %f\n", create_ti);

	start = clock();
	store_raw_chunk(id, smallBuff, 40);
	end = clock();
	storeRaw_ti = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("\nstore_raw_chunk(small) time: %f\n", storeRaw_ti);

	start = clock();
	store_raw_chunk(id, medBuff, 200);
	end = clock();
	storeRaw_ti = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("\nstore_raw_chunk(medium) time: %f\n", storeRaw_ti);

	start = clock();
	store_raw_chunk(id, largeBuff, MAX_BUFF_SIZE);
	end = clock();
	storeRaw_ti = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("\nstore_raw_chunk(large) time: %f\n", storeRaw_ti);

	start = clock();
	read_raw_chunk(id, &meta_b, readinBuff, MAX_BUFF_SIZE);
	end = clock();
	readOut_ti = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("\nread_raw_chunk() time: %f\n", readOut_ti);

	start = clock();
	store_processed_chunk(id, smallBuff, 40);
	end = clock();
	storeProc_ti = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("\nstore_processed_chunk(small) time: %f\n", storeProc_ti);

	start = clock();
	store_processed_chunk(id, medBuff, 200);
	end = clock();
	storeProc_ti = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("\nstore_processed_chunk(medium) time: %f\n", storeProc_ti);

	start = clock();
	resp0 = store_processed_chunk(id, largeBuff, MAX_BUFF_SIZE);
	end = clock();
	storeProc_ti = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("\nstore_processed_chunk(large) time: %f\n", storeProc_ti);
	printf("\nresponse: %d", resp0);

	return 0;
}*/

int throughput_test() {
	FS_Init();

	char *buff1 = "FirstBuffMatey";
	char *buff2 = "SecondBuffMate";
	char *buff3 = "AnottherBuff";
	char *buff4 = "SomethingElse.";

	char readoutBuff[MAX_TRFBUFF_SIZE];
	char readoutBuff2[MAX_PRDATBUFF_SIZE];
	trf_header_t readoutHeader;
	trf_header_t readoutHeader2;

	//store all of 1
	int stream1 = create_new_rawstream(1);
	store_raw_chunk(stream1, buff1, 7);
	store_raw_chunk(stream1, buff2, 7);
	store_raw_chunk(stream1, buff3, 6);
	store_raw_chunk(stream1, buff4, 7);

	//store and do part of 2
	/*int stream2 = create_new_rawstream(1);
	store_raw_chunk(stream2, buff1, 6);
	checkout_raw_chunk(stream2, readoutBuff, &readoutHeader);
	//printf("%s\n", readoutBuff);
	store_processed_chunk(stream2, readoutBuff, readoutHeader.payload);
	//printf("readout payload: %d", readoutHeader.payload);
	memset(readoutBuff, 0, readoutHeader.payload);
	store_raw_chunk(stream2, buff2, 7);
	store_raw_chunk(stream2, buff3, 6);
	store_raw_chunk(stream2, buff4, 7);

	//do rest of 2
	checkout_raw_chunk(stream2, readoutBuff2, &readoutHeader2);
	//printf("%s\n", readoutBuff2);
	store_processed_chunk(stream2, readoutBuff2, readoutHeader2.payload);
	checkout_raw_chunk(stream2, readoutBuff, &readoutHeader);
	//printf("%s\n", readoutBuff);
	store_processed_chunk(stream2, readoutBuff, readoutHeader.payload);
	checkout_raw_chunk(stream2, readoutBuff, &readoutHeader);
	//printf("%s\n", readoutBuff);
	store_processed_chunk(stream2, readoutBuff, readoutHeader.payload);*/

	checkout_raw_chunk(stream1, readoutBuff, &readoutHeader);
	print_trf_buff(readoutHeader);
	store_processed_chunk(stream1, readoutBuff, readoutHeader.payload);
	memset(readoutBuff, 0, readoutHeader.payload);
	checkout_raw_chunk(stream1, readoutBuff, &readoutHeader);
	store_processed_chunk(stream1, readoutBuff, readoutHeader.payload);
	checkout_raw_chunk(stream1, readoutBuff, &readoutHeader);
	store_processed_chunk(stream1, readoutBuff, readoutHeader.payload);
	checkout_raw_chunk(stream1, readoutBuff, &readoutHeader);
	store_processed_chunk(stream1, readoutBuff, readoutHeader.payload);
    
	cap_rawstream(stream1);
	//cap_rawstream(stream2);
	cap_processed_file(stream1, 0);
	//cap_processed_file(stream2, 0); 

	print_base_buff(stream1);
	//print_base_buff(stream2);
	return 0;
}


int complete_main_unit_test() {
	int id;
	id = create_new_rawstream(1); //create a rawstream of 1 sample/second

	//put stream generation function here
	//generate 4 chunks of data
	//read out 2 chunks of data for transform
	//write transform data back
	//generate 5th chunk of data and store raw chunk
	//write and store final 3 transformed files
	//check prdat file 

	return 0;
}

int main() {

	//ADD NEW TEST BATTERY FUNCTIONS
	int(*test_battery[])() = {//filestream_create_test,
							  //store_raw_chunk_test,
							  //store_raw_chunk_test,
							  //store_raw_chunk_test,
							  //read_two_chunk_test
							  throughput_test
							  /*test_cap_rawstream,*/
							  /*timing_test,*/
							  /*store_procbuff_test*/};
	
	//Test battery call
	int i;
	int result;
	for (i = 0; i < NUM_TESTS; i++) {
		printf("------------------\nTest %d\n\nTest Log:\n", (i+1));
		result = (*test_battery[i])();
		printf("\nTest Pass: %d\n\n", result);
	}

}

//helper functions
void print_base_buff(int id){
		char target_file[LONG_FNAME_LENGTH];
		trfb_header_t base;
		sprintf(target_file, "storage/%d.trf", id);
		FILE *f_ptr = fopen(target_file, "rb");
        fread(&base, sizeof(trfb_header_t), 1, f_ptr);
		printf("*~trfb file:\nid: %d\nrate: %d\nflink: %s\nllink: %s\nnlinks: %d\nrlink: %s\nruntime: %lf\nbuild: %d\n",
		 base.stream_id, base.sample_rate, base.first_file, base.last_file, base.num_links, base.readout_ptr, base.run_time, base.build_state);
}

void print_trf_buff(trf_header_t trgt) {
	printf("TRF HEADER: \n");
	printf("id: %d\n", trgt.stream_id);
	printf("sample rate: %d\n", trgt.sample_rate);
	printf("point in stream time: %lf\n", trgt.time_slice);
	printf("payload size (bytes): %d\n", trgt.payload);
	printf("previous link: %s\n",trgt.prev_file);
	printf("next link: %s\n", trgt.next_file);
	printf("link number: %d\n\n", trgt.link_id);
}
