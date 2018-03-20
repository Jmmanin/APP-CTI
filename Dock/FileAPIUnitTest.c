//The unit test driver for testing the functionalities of the file read, write system

#include <stdlib.h>
#include <stdio.h>
#include "FileAPI.h"

#define NUM_TESTS 1  /*Tests in the battery*/

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
    char *buffer1 = "Test1";
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
    printf("id: %d", id);
    if(id)
    {
		
		/*NOTE: The function expects chunk_timelength as a value in seconds, not buffer length itself*/
        response1 = store_raw_chunk(id, buffer1, 5); //Everything is set up to assume 1 letter per second
        response2 = store_raw_chunk(id, buffer2, 8);                
        
		printf("store1 exit code: %d\n", response1);
		printf("store2 exit code: %d\n\n", response2);

		sprintf(target_file, "%d_0.trf", id);
		FILE *f_ptr = fopen(target_file, "rb");
        fread(&header1, sizeof(trf_header_t), 1, f_ptr);
        fread(&buffer_cmp1, sizeof(char), header1.payload, f_ptr);
        fclose(f_ptr);

		printf("_0.trf: id %d\n rate: %d\n timeslice: %d\n payload: %d\n pfile: %s\n nfile: %s\n\n",
     		header1.stream_id, header1.sample_rate, header1.time_slice, header1.payload, header1.prev_file, header1.next_file);

		sprintf(target_file, "%d_1.trf", id);
		f_ptr = fopen(target_file, "rb");
        fread(&header2, sizeof(trf_header_t), 1, f_ptr);
        fread(&buffer_cmp2, sizeof(char), header1.payload, f_ptr);
        fclose(f_ptr);

		printf("_1.trf: id %d\n rate: %d\n timeslice: %d\n payload: %d\n pfile: %s\n nfile: %s\n\n",
     		header2.stream_id, header2.sample_rate, header2.time_slice, header2.payload, header2.prev_file, header2.next_file);	
        
		printf("Buffer0:%s!\n", buffer_cmp1);
		printf("Buffer1:%s!\n", buffer_cmp2);
    }
    
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
	int(*test_battery[])() = {/*filestream_create_test, */store_raw_chunk_test};
	
	//Test battery call
	int i;
	int result;
	for (i = 0; i < NUM_TESTS; i++) {
		printf("------------------\nTest %d\n\nTest Log:\n", (i+1));
		result = (*test_battery[i])();
		printf("\nTest Pass: %d\n\n", result);
	}

}
