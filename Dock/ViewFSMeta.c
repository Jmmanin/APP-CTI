//Tool for viewing meta files in the file system
#include "UniversalDefines.h"
#include "FileAPI.h"
#include <stdio.h>
#include <stdlib.h>



int main() {
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

    printf("\n\ndone.\n");
    return 0;
}