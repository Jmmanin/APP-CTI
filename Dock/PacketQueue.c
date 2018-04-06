//Queue module. Separated for readability

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "UniversalDefines.h"

char *dataArray;    //malloc a dummy pointer
int packetSize;     //packet length in chars
int q_length;       //length of q in number of packets
int front = 0;
int rear = -1;
int itemCount = 0;

//prototypes
int Q_isFull();
void Q_resizeAndAdd(char *data);
int Q_getPacketSize();


void Q_Init(int new_size) {
    q_length = Q_START_SIZE;
    front = 0;
    rear = -1;
    itemCount = 0;
    packetSize = new_size;
    dataArray = malloc((packetSize*q_length));

}

void Q_reset() {
    free(dataArray);
    Q_Init(Q_getPacketSize());
}

int Q_getPacketSize() {
    return packetSize;
}

int Q_isEmpty() {
   if(itemCount == 0) {
       return 1;
   } 
   return 0;
}

int Q_size() {
   return itemCount;
}  

void Q_peek(char *output_buff) {
    int i;
    for(i = 0; i < packetSize; i++) {
        output_buff[i] = dataArray[front+i];
    }
}

void Q_addData(char *data) {
    int i;

    if(!Q_isFull()) {
        if(rear == q_length-1) {
            rear = -1;            
        } 
        rear++;      
      
        for(i = 0; i < packetSize; i++) {
            dataArray[(rear*packetSize)+i] = data[i];
        }
        itemCount++;

    } else {
       Q_resizeAndAdd(data);
   }

}

void Q_removeData(char *output_buff) {
    int i;
    for(i = 0; i < packetSize; i++) {
        dataArray[(front*packetSize)+i] = output_buff[i];
    }
    front++;
	
    if(front == q_length) {
        front = 0;
    }
	
   itemCount--;
}

void Q_resizeAndAdd(char *data) {
    int i, payload_qty;
    char * new_dataArray = malloc(2*packetSize*q_length);

    payload_qty = q_length * packetSize;
    for(i = 0; i < payload_qty; i++) {
        new_dataArray[i] = dataArray[i];
    }

    free(dataArray);
    dataArray = new_dataArray;
    q_length = q_length * 2;
    
    Q_addData(data);
}


int Q_isFull() {
   if(itemCount == q_length) {
       return 1;
   } 
   return 0;
}
