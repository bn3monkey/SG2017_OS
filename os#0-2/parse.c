#include "parse.h"

// desc    : tokenize the string of cmd
// return  : if there is invalid command, return INVALIDCMD. if not, return 0
int tokenize(int* argc, char*** argv, char* cmd)
{
	int index;
	*argc = 0;
	
	//1. tokenize First Word by white space.
	index = tokenFirstWord(argc, argv, cmd, " \t\n");
	//2. if first word is not invalid, return INVALIDCMD
	//example : all white space or
	//the first character that is not white space is comma
	if (index == INVALIDCMD)
		return INVALIDCMD;
	
	//3. tokenize the next word while meeting the line end of command 
	for(;index!=-1;)
	{
		index = tokenNextWord(argc, argv, cmd, " \t\n", index);	
	}
	return 0;
}
// desc    : get the fisrt word of cmd 
// return  : the next value of the index of the token symbol.
//           if token symbol is '\0', return -1. it means that the string is over
int tokenFirstWord(int* argc, char*** argv, char* source, char* space)
{
	int start, end, j;
	char* dest;

	//1. remove the front whitespace at the command line 
	start = removeSpaceFirst(source, " \t\n", 0, (int)strlen(source));
	
	//2. if there is all white space or no input, return -1(end of line)
	if (source[start] == '\0')
		return INVALIDCMD;

	//3. find the index of command line that is whitespace.
	// if there is comma, return INVALIDCMD
	for (end = start; source[end] != '\0'; end++)
	{
		for (j = 0; space[j] != '\0'; j++)
		{
			if (space[j] == source[end])
				break;
		}
		if (space[j] != '\0')
			break;
		//if (source[end] == ',')
		//	return INVALIDCMD;
	}

	//4. By using the index of start and end, make string which is the first word of command line
	dest = makeStr(source, start, end);

	//5. push the string to argument
	inputArgv(argc, argv, dest);

	for (; source[end] != '\0'; end++)
	{
		for (j = 0; space[j] != '\0'; j++)
		{
			if (space[j] == source[end])
				break;
		}
		if (space[j] == '\0')
			break;
		//if (source[end] == ',')
		//	return INVALIDCMD;
	}


	//6. if meet the end of line, return -1
	if (source[end] == '\0')
		return -1;
	//7. return the next value of the index
	return end;
}
// desc    : get the next word of cmd 
// return  : the next value of the index of the token symbol.
//           if token symbol is '\0', return -1. it means that the string is over
int tokenNextWord(int* argc, char*** argv, char* source, char* space, int start)
{
	int end, j;
	char* dest;

	//1. remove the front whitespace at the command line 
	//start = removeSpaceFirst(source, " \t\n", start, (int)strlen(source));
	
	//2. if there is all white space or no input, return -1(end of line)
	if (source[start] == '\0')
		return -1;

	//3. find the index of command line that is whitespace.
	// if there is comma, return INVALIDCMD
	for (end = start; source[end] != '\0'; end++)
	{
		for (j = 0; space[j] != '\0'; j++)
		{
			if (space[j] == source[end])
				break;
		}
		if (space[j] != '\0')
			break;
		//if (source[end] == ',')
		//	return INVALIDCMD;
	}

	//4. By using the index of start and end, make string which is the first word of command line
	dest = makeStr(source, start, end);

	//5. push the string to argument
	inputArgv(argc, argv, dest);

	for (; source[end] != '\0'; end++)
	{
		for (j = 0; space[j] != '\0'; j++)
		{
			if (space[j] == source[end])
				break;
		}
		if (space[j] == '\0')
			break;
		//if (source[end] == ',')
		//	return INVALIDCMD;
	}


	//6. if meet the end of line, return -1
	if (source[end] == '\0')
		return -1;
	//7. return the next value of the index
	return end;
}
// desc    : remove whitespace in the front part of the string
void removeSpace(char** dest, char* source, char* space, int prevstart, int prevend)
{
	int start, end;
	//1. Remove the front part of whitespace
	start = removeSpaceFirst(source, space, prevstart, prevend);
	//2. Remove the last part of whitespace
	end = removeSpaceEnd(source, space, start, prevend);
	//3. make the string removed whitespace
	*dest = makeStr(source, start, end);
	return;
}
// desc    : return the index of the first part that the string value is not whitespace 
// return  : ``
int removeSpaceFirst(char* source, char* space, int prevstart, int prevend)
{
	int start, j;
	for (start = prevstart; start < prevend; start++)
	{
		for (j = 0; space[j] != '\0'; j++)
			if (space[j] == source[start])
				break;
		if (space[j] == '\0')
			break;
	}
	return start;
}
// desc    : return the index of the end part that the string value is not whitespace 
// return  : ``
int removeSpaceEnd(char* source, char* space, int prevstart, int prevend)
{
	int end, j;
	for (end = prevend; end >= prevstart; end--)
	{
		for (j = 0; space[j] != '\0'; j++)
			if (space[j] == source[end-1])
				break;
		if (space[j] == '\0')
			break;
	}
	return end;
}

// desc    : copy the string
char* makeStr(char* source, int start, int end)
{
	int i, j;
	char* dest;
	dest = (char *)malloc(sizeof(char)*(end - start + 1));
	for (i = start, j = 0; i < end; j++, i++)
		dest[j] = source[i];
	dest[j] = '\0';
	return dest;
}
// desc    : copy the string to the argument
void inputArgv(int* argc, char*** argv, char* source)
{
	if (*argc == 0)
		*argv = (char **)malloc(sizeof(char *) * ((*argc) + 1));
	else
		*argv = (char **)realloc(*argv, sizeof(char *) * ((*argc) + 1));

	(*argv)[(*argc)++] = source;
}

// desc    : Free Argv
bool deletetokenized(int* argc, char*** argv)
{
    int i=0;
    if(*argv == NULL)
        return false;
   
    for(i=0;i<*argc;i++)
    {
        if((*argv)[i] != NULL)
	{
            free((*argv)[i]);
	}
    }
    free(*argv);
    *argv = NULL;
    *argc = 0;
    return true;
}




// desc   : parse a string that represents hexa integer to integer
// make exception if a string is not number or integer maked is over boundary

int parseHexa_long(char* str)
{
		int i, value;
		if(str[0]==',')
				return NONUMBER;
		for(value = i = 0; str[i]!='\0' && str[i] != ','; i++)
		{
				if((str[i] >= '0' && str[i] <= '9') ||
								(str[i] >= 'a' && str[i] <= 'f') ||
								(str[i] >= 'A' && str[i] <='F'))
				{
						value *= 16;
						if(str[i] >= '0' && str[i] <= '9')
								value += str[i] - '0';
						if(str[i] >= 'a' && str[i] <= 'f')
								value += str[i] - 'a' + 10;
						if(str[i] >= 'A' && str[i] <='F')
								value += str[i] - 'A' + 10;
				}
				else
						return NONUMBER;
		}
		return value;
}

// desc   : parse a string and index of start and end of string that represents hexa integer to integer
// return : if error, NO NUMBER. or integer
int parseHexa(char* str, int start, int end)
{
    int i = start,value = 0;
    for(;i<=end;i++)
    {
        if((str[i] >= '0' && str[i] <= '9') ||
           (str[i] >= 'a' && str[i] <= 'f') ||
           (str[i] >= 'A' && str[i] <='F'))
        {
            value *= 16;
            if(str[i] >= '0' && str[i] <= '9')
                value += str[i] - '0';
            if(str[i] >= 'a' && str[i] <= 'f')
                value += str[i] - 'a' + 10;
            if(str[i] >= 'A' && str[i] <='F')
                value += str[i] - 'A' + 10;
            
            
        }
        else
            return NONUMBER;
        
    }
    return value;
    
}

// desc   : parse a string and index of start and end of string that represents decimal integer to integer
// return : if error, NO NUMBER. or integer
int parseInt(char* str, int start, int end)
{
    int i = start,value = 0;
    for(;i<=end;i++)
    {
        if(str[i] >= '0' && str[i] <= '9')
        {
            value *= 10;
            if(str[i] >= '0' && str[i] <= '9')
                value += str[i] - '0';         
        }
        else
            return NONUMBER;
        
    }
    return value;
 
}


// desc   : change lower alphabet of string to upper alphabet of string
char* strupr(char* str)
{
		int i;
		for(i=0; str[i]!='\0'; i++)
				if(str[i]>='a' && str[i]<='z')
						str[i] -= 32;
		return str;
}

// desc   : change lower alphabet of string to upper alphabet of string
char* strlow(char* str)
{
    int i;
    for(i=0; str[i]!='\0'; i++)
            if(str[i]>='A' && str[i]<='Z')
                    str[i] += 32;
    return str;
}
