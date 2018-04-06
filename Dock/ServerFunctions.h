//Functions used by the workers of the server, abstracted into a module
//inclusion guard
#ifndef SRVR_FUNC_INCLUDE
#define SRVR_FUNC_INCLUDE

//inclusions
#include "FileAPI.h"
#include "UniversalDefines.h"


//************************
//  Input Worker Functions
//************************
    //validates that the input char is in the cmd list (add new cmds here)
int validate_cmd(char cmd);

    //prints out descriptions of the valid server control cmds (add new cmd descriptions here)
void print_cmdls();

//************************
//  Dock Worker Functions
//************************
int D_add_packet(char *hold_buff, char *new_pkt, int num_pkts);
//************************
//  Transform Worker Functions
//************************
void T_data_transform(char *work_in, char *work_out, int work_size);
//************************
//  DPS Worker Functions
//************************

#endif