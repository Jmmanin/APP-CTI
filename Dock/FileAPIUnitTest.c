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

	int id_1 = create_new_rawstream();
	int id_2 = create_new_rawstream();
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
    char *buffer2 = "Test2"; 
    char buffer_cmp1[5];  
    //trf_header_t header;

    FS_Init();
    printf("store raw chunk test:\n");

    int id = create_new_rawstream();
    
    if(id)
    {
        store_raw_chunk(id, buffer1, 5);
        store_raw_chunk(id, buffer2, 5);                
        /*FILE *f_ptr = fopen("2_0.trf", "r");
        fread(&header, sizeof(trf_header_t), 1, f_ptr);
        fread(&buffer_cmp1, sizeof(char), 5, f_ptr);
        fclose(f_ptr);
        printf("Buffer:%s!\n", buffer_cmp1);*/ 
    }
    
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
