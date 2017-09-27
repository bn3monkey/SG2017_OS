#include "shell.h"
#include <stdio.h>

//Main Program Cycle
void Cycle()
{
    int state;
    Argument arg;

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
    return STATE_INCOMPLETE;
}
int shell_hash(Argument arg)
{
    return STATE_INCOMPLETE;
}
int shell_bitmap(Argument arg)
{
    return STATE_INCOMPLETE;
}
void shell_nothing(Argument arg)
{
    return;
}