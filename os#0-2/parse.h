#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#define INVALIDCMD -1000
#define NONUMBER -1001


// desc    : tokenize the string of cmd
// return  : if there is invalid command, return INVALIDCMD. if not, return 0
int tokenize(int* argc, char*** argv, char* cmd);

// desc    : get the fisrt word of cmd 
// return  : the next value of the index of the token symbol.
//           if token symbol is '\0', return -1. it means that the string is over
int tokenFirstWord(int* argc, char*** argv, char* source, char* space);
// desc    : get the next word of cmd 
// return  : the next value of the index of the token symbol.
//           if token symbol is '\0', return -1. it means that the string is over
int tokenNextWord(int* argc, char*** argv, char* source, char* space, int start);

// desc    : remove whitespace in the front part of the string
void removeSpace(char** dest, char* source, char* space, int prevstart, int prevend);
// desc    : return the index of the first part that the string value is not whitespace 
// return  : ``
int removeSpaceFirst(char* source, char* space, int prevstart, int prevend);
// desc    : return the index of the end part that the string value is not whitespace 
// return  : ``
int removeSpaceEnd(char* source, char* space, int prevstart, int prevend);

// desc    : copy the string
char* makeStr(char* source, int start, int end);
// desc    : copy the string to the argument
void inputArgv(int* argc, char*** argv, char* source);

// desc    : Free Argv
bool deletetokenized(int* argc, char*** argv);


/* About string to int */

// desc   : parse a string that represents hexa integer to integer
int parseHexa_long(char* str);

// desc   : parse a string and index of start and end of string that represents hexa integer to integer
// return : if error, NO NUMBER. or integer
int parseHexa(char* str, int start, int end);

// desc   : parse a string and index of start and end of string that represents decimal integer to integer
// return : if error, NO NUMBER. or integer
int parseInt(char* str, int start, int end);

/* About Spelling */

// desc   : change lower alphabet of string to upper alphabet of string
char* strupr(char* str);
// desc   : change lower alphabet of string to upper alphabet of string
char* strlow(char* str);
