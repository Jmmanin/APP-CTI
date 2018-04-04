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
  float serial_angle[5];
  float serial_pressure[5];
  float serial_orientation[3];
  int32_t serial_temp;
} packet_to_recv;

int main()
{ 
  int32_t start_val = -1386103603;
  int32_t start_recvd = 0;
  uint8_t start_buf = 0;
  uint32_t temp = 0;
  
  FILE *f_ptr = fopen("/dev/cu.usbserial-DN03FU8O", "r");

  signal(SIGINT, handler);
  
  while(run)
  {
    if(start_recvd == start_val) //if start val received
    {
      fread(&packet_to_recv, sizeof(struct serial_packet), 1, f_ptr); //read packet
    
      printf("====================================\n");
      printf("Orientation:\n");
      printf("X: %f\n", packet_to_recv.serial_orientation[0]);
      printf("Y: %f\n", packet_to_recv.serial_orientation[1]);
      printf("Z: %f\n", packet_to_recv.serial_orientation[2]);
      printf("====================================\n\n");
    
      start_recvd = 0;
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
