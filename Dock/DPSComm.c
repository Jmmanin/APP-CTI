//Functions for communicating to the DPS

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include "UniversalDefines.h"

#define BACKLOG_LENGTH 10
#define MAX_DPS_IMSG_LENGTH 256
#define MAX_DPS_OMSG_LENGTH 1024

int DPSC_validate_cmd_type(char op);
int DPS_close_port();

//module globals. (Means we can only hold 1 connection but thats fine)
int initd = 0;
int sockfd = 0, connfd = 0;
struct sockaddr_in serv_addr;

//sets up the server side connection
void DPS_socketInit() {
    if(initd == 1) {
        return;
    }
    //initialize the socket struct
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(4648);

    //set up the socket to listen on the port
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        printf("*** Error creating socket. DPS Comm not init'd\n");
    }

    bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    listen(sockfd, BACKLOG_LENGTH);
}

int DPS_getClient() {
    connfd = accept(sockfd, (struct sockaddr*) NULL, NULL);
    if(connfd != 0) {
        return 1;
    }
    return 0;
}

//does a communication with the client to establish what type of comm is occurring.
int DPS_getClientState() {
    char operation_type;
    char *ack = "a";
    int kill_flag = 0;

    //connfd = accept(sockfd, (struct sockaddr*) NULL, NULL);
    read(connfd, &operation_type, sizeof(char));
    printf("client raw ask: %d\n", operation_type);
    if(!DPSC_validate_cmd_type(operation_type)) {
        ack[0] = 'n';
        operation_type = 0;
        kill_flag = 1;
    }

    write(connfd, &ack[0], 1);
    if(kill_flag) {
        DPS_close_port();
    }
    if(operation_type == 108) {
        return 2;
    } else if(operation_type == 112) {
        return 1;
    } 
    return 0;
    
}

int DPS_readStreamAddr(int *stream_id) {
    char ack = 'a';
    char int_buff[sizeof(int)+1]; //+1 for null terminator
    read(connfd, int_buff, sizeof(int));
    //printf("recieved raw id: %s\n", int_buff);
    *stream_id = atoi(int_buff);
    //printf("converted id: %d\n", *stream_id);
    write(connfd, &ack, 1);
    return 0;
}

int DPS_returnStreamPayload(int payload_length, char *payload) {
    //convert sizing to ascii string
    char payload_out[16];
    int i;
    for(i = 0; i < 16; i++) {
        payload_out[i] ='\0';
    }
    sprintf(payload_out, "%d", payload_length);
    //printf("string encoding int: '%s' as", payload_out);

    //printf("\n");

    //send over the size of the payload
    write(connfd, payload_out, 16);

    if(payload_length == 0) {
        return 0;
    }

    //send over the full payload
    write(connfd, payload, payload_length);
    return 0;
}

int DPS_sendPacket(char *pkt) {
    write(connfd, pkt, TRNS_PKT_SIZE);

}

int DPS_check_release() {
    char release_confirm;
    read(connfd, &release_confirm, sizeof(char));
    printf("release confirm %c, %x\n", release_confirm, release_confirm);
    if(release_confirm == 'r') {
        return 1;
    }
    if(release_confirm == 'k') {
        return 2;
    }
    return 0;
}

int DPS_close_port() {
    if(connfd != 0) {
        close(connfd);
    }
    return 0;
}

//-helper functions----------------------------
int DPSC_validate_cmd_type(char op) {
    if(op != 108 && op != 112) {
        return 0;
    }
    return 1;
}