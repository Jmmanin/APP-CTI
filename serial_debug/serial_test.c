/*
*This program outputs data from serial as a continuous string of characters
*Use this program to test the serial connection with the rig
*particularly with rig_human_readable.ino if issues with the rig connection occur
*
*Takes 1 argument- string, absolute file path to serial port file
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  if(argc!=2) //checks for proper number of command line args
  {
    fprintf(stderr,"Usage: ./serial_test <serial port>\n");
    return(1);
  }

  char c;
  
  FILE *f_ptr = fopen(argv[1], "r");
  while(1)
  {
    c = fgetc(f_ptr);
    if(feof(f_ptr))
      break;
    printf("%c", c);
  }
  
  fclose(f_ptr);
  return(0);
}
