//header for the packet queue module

#ifndef PKTQ_INCLUDE
#define PKTQ_INCLUDE

//Initializes the Q to work with the specified packet size
void Q_Init(int new_size);

//Returns the current size of the expected packets in bytes
int Q_getPacketSize();

//Returns 1 if the Q is empty, 0 if it is not
int Q_isEmpty();

//Returns the current number of packets in the Q
int Q_size();  

//writes out a copy of the next value to be read out of the queue
// to the provided buffer
void Q_peek(char *output_buff);

//adds the data which is assumed to be 1 standard packet to the Q, 
// resizing if necessary
void Q_addData(char *data);

//pulls the next value from the queue
void Q_removeData(char *output_buff);

//clears the Q
void Q_reset();

#endif