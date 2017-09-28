#include "shell.h"
#include <stdio.h>

//#define WHILE_LIMIT

//Main Program Cycle
void Cycle()
{
    int state;
    Argument arg;

    srand(time(NULL));
    for(state=0;state<10;state++)
    {
        m_list[state] = NULL;
        m_hash[state] = NULL;
        m_bitmap[state] = NULL;
    }

    while (true)
    {
        arg = NewCommand();
        state = shell(arg);

        if (arg.argc > 0)
        {
            if (state == STATE_COMPLETE)
            {
                //Nothing to do

                //For Debugin
                //printf("OK!\n");
            }
            else if (state == STATE_INCOMPLETE)
            {
                printf("There is no instruction : %s\n", arg.argv[0]);
            }
            else if (state == STATE_NONSTRUCTION)
            {
                printf("There is no instruction : %s\n", arg.argv[0]);
            }
            else if (state == STATE_NOPARAMETER)
            {
                printf("There is no accurate parameter : %s\n", arg.argv[0]);
            }
            else if (state == STATE_NOINT)
            {
                printf("There is no accurate integer : %s\n", arg.argv[0]);
            }
            else if (state == STATE_EXCESSINT)
            {
                printf("The integer range excess: %s\n", arg.argv[0]);
            }
            #ifdef DEBUG_SHELL
            else if (state == STATE_DATANULL)
            {
                printf("The Data Structure is not created : %s\n", arg.argv[1]);
            }
            #endif

            //state Section
            if (state == STATE_QUIT)
            {
                DeleteCommand(&arg);
                return;
            }
        }
        DeleteCommand(&arg);

	#ifdef WHILE_LIMIT
	int count;
	count=0;
	count++;
	if(count >= 10000)
		return;
	#endif
    }
}
//Input using stdin and Make Argument
Argument NewCommand()
{
    Argument temp;
    temp.argc = 0;
    temp.argv = NULL;
    static char buf[1001];
    while (true)
    {
        fgets(buf, 1000, stdin);
	buf[strlen(buf)-1] = '\0';
        if (tokenize(&temp.argc, &temp.argv, buf) != INVALIDCMD)
            break;
        deletetokenized(&(temp.argc), &(temp.argv));
    }
    return temp;
}
//Delete Old Command
void DeleteCommand(Argument *arg)
{
    deletetokenized(&(arg->argc), &(arg->argv));
}

//Receiving Argument and find the appropriate function and conduct it
int shell(Argument arg)
{
    int temp = STATE_INCOMPLETE;
    temp = shell_integrated(arg);
    if (temp != 0)
        return temp;
    temp = shell_list(arg);
    if (temp != 0)
        return temp;
    temp = shell_hash(arg);
    if (temp != 0)
        return temp;
    temp = shell_bitmap(arg);
    if (temp != 0)
        return temp;
    return temp;
}
int shell_integrated(Argument arg)
{
    int temp = STATE_INCOMPLETE;
    temp = ins_create(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_delete(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_dumpdata(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_quit(arc, arv);
    if (temp != 0)
        return temp;
    return temp;
}
int shell_list(Argument arg)
{
    int temp = STATE_INCOMPLETE;

    temp = ins_list_pop_front(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_list_pop_end(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_list_front(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_list_back(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_list_size(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_list_empty(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_list_max(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_list_min(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_list_reverse(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_list_sort(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_list_push_back(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_list_push_front(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_list_insert_ordered(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_list_remove(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_list_unique(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_list_insert(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_list_swap(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_list_splice(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_list_shuffle(arc, arv);
    if( temp != 0)
        return temp;
    return STATE_INCOMPLETE;
}
int shell_hash(Argument arg)
{
    int temp = STATE_INCOMPLETE;

    temp = ins_hash_empty(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_hash_size(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_hash_clear(arc, arv);
    if (temp != 0)
      return temp;
    
    temp = ins_hash_insert(arc, arv);
    if (temp != 0)
      return temp;
    temp = ins_hash_delete(arc, arv);
    if (temp != 0)
        return temp;
    temp = ins_hash_find(arc, arv);
    if (temp != 0)
      return temp;
    temp = ins_hash_replace(arc, arv);
    if (temp != 0)
         return temp;
    
    temp = ins_hash_apply(arc, arv);
    if (temp != 0)
        return temp;
    return STATE_INCOMPLETE;
}
int shell_bitmap(Argument arg)
{
    int temp = STATE_INCOMPLETE;

    temp =  ins_bitmap_dump(arc, arv);
    if (temp != 0)
        return temp;
    temp =  ins_bitmap_size(arc, arv);
    if (temp != 0)
        return temp;
    
    temp =  ins_bitmap_set_all(arc, arv);
    if (temp != 0)
        return temp;
    
    temp =  ins_bitmap_mark(arc, arv);
    if (temp != 0)
    {
         return temp;
    }
    temp =  ins_bitmap_expand(arc, arv);
    if (temp != 0)
        return temp;
    temp =  ins_bitmap_flip(arc, arv);
    if (temp != 0)
         return temp;
    temp =  ins_bitmap_reset(arc, arv);
    if (temp != 0)
        return temp;
    temp =  ins_bitmap_test(arc, arv);
    if (temp != 0)
        return temp;
    
    temp =  ins_bitmap_all(arc, arv);
    if (temp != 0)
         return temp;
    temp =  ins_bitmap_any(arc, arv);
    if (temp != 0)
        return temp;
    temp =  ins_bitmap_none(arc, arv);
    if (temp != 0)
        return temp;
    
    temp =  ins_bitmap_set(arc, arv);
    if (temp != 0)
        return temp;
    
    temp =  ins_bitmap_contains(arc, arv);
    if (temp != 0)
        return temp;
    temp =  ins_bitmap_count(arc, arv);
    if (temp != 0)
         return temp;
    temp =  ins_bitmap_scan(arc, arv);
    if (temp != 0)
        return temp;
    temp =  ins_bitmap_scan_and_flip(arc, arv);
    if (temp != 0)
        return temp;
    temp =  ins_bitmap_set_multiple(arc, arv);
    if (temp != 0)
        return temp;

    return STATE_INCOMPLETE;
}
void shell_nothing(Argument arg)
{
    return;
}
