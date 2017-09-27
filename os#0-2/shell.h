#ifndef __SHELL__
#define __SHELL__

#include "instruction.h"

#define arc (arg.argc)
#define arv (arg.argv)

#define DEBUG_SHELL

typedef struct _argument {
    int argc;
    char** argv;    
} Argument;


//Main Program Cycle
void Cycle();
//Input using stdin and Make Argument
Argument NewCommand();
//Delete Old Command
void DeleteCommand(Argument* arg);
//Receiving Argument and find the appropriate function and conduct it
int shell(Argument arg);

int shell_integrated(Argument arg);
int shell_list(Argument arg);
int shell_hash(Argument arg);
int shell_bitmap(Argument arg);
void shell_nothing(Argument arg);
#endif
