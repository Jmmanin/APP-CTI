/*
*This program reads in serial packets and outputs the contents
*Use this program to test the serial connection with the rig
*particularly with rig_serial.ino if issues with the rig connection occur
*
*Takes 1 argument- string, absolute file path to serial port file
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>

static volatile int run = 1;

void handler() //handles ^C
{
  run = 0;
}

struct serial_packet
{
  float serial_angle[4];
  float serial_pressure[5];
  float serial_orientation[3];
  int32_t serial_temp;
} packet_to_recv;

int main(int argc, char **argv)
{ 
  int32_t start_val = -1386103603;
  int32_t start_recvd = 0;
  uint8_t start_buf = 0;
  uint32_t temp = 0;

  if(argc!=2) //checks for proper number of command line args
  {
    fprintf(stderr,"Usage: ./serial_recv <serial port>\n");
    return(1);
  }
  
  FILE *f_ptr = fopen(argv[1], "r");

  signal(SIGINT, handler);
  
  while(run)
  {
    if(start_recvd == start_val) //if start val received
    {
      fread(&packet_to_recv, sizeof(struct serial_packet), 1, f_ptr); //read packet
    
      printf("====================================\n"); //print packet contents
      printf("Angle:\n");
      printf("Index: %f\n", packet_to_recv.serial_angle[0]);
      printf("Middle: %f\n", packet_to_recv.serial_angle[1]);
      printf("Ring: %f\n", packet_to_recv.serial_angle[2]);
      printf("Little: %f\n", packet_to_recv.serial_angle[3]);
      printf("Pressure:\n");
      printf("Thumb-most: %f\n", packet_to_recv.serial_pressure[0]);
      printf("Palm 1: %f\n", packet_to_recv.serial_pressure[1]);
      printf("Palm 2: %f\n", packet_to_recv.serial_pressure[2]);
      printf("Palm 3: %f\n", packet_to_recv.serial_pressure[3]);
      printf("Little-most: %f\n", packet_to_recv.serial_pressure[4]);
      printf("Orientation:\n");
      printf("X: %f\n", packet_to_recv.serial_orientation[0]);
      printf("Y: %f\n", packet_to_recv.serial_orientation[1]);
      printf("Z: %f\n", packet_to_recv.serial_orientation[2]);
      printf("Temperature:\n");
      printf("%d *C\n", packet_to_recv.serial_temp);
      printf("====================================\n\n");
    
      start_recvd = 0; //reset start value buffer
    }
    else //reads in bytes until start val is found
    {
      fread(&start_buf, sizeof(uint8_t), 1, f_ptr); //reads byte
      start_recvd = (uint32_t)start_recvd >> 8; //shift inserts byte
      temp = (uint32_t)start_buf << 24;
      start_recvd = start_recvd | temp;
    }
  }
  
  printf("Serial closed.\n");
  fclose(f_ptr);
  return(0);
}
