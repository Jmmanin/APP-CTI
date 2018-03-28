#include <stdio.h>
#include <stdlib.h>

int main()
{
  char c;
  
  FILE *f_ptr = fopen("/dev/ttyUSB0", "r");
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
