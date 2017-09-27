#include "instruction.h"

#define DEBUG

struct list *m_list[10];
struct hash *m_hash[10];
struct bitmap *m_bitmap[10];

static int getNum(char *ds, char *name)
{
    int start, end;
    start = strlen(ds);
    end = strlen(name);
    if (start == end)
        return NONUMBER;
    return parseInt(name, start, end - 1);
}

//The instruction form in shell like
// command data_structure data_structure_name [bit_count]
static int template_1(const char *ins, int argc, char **argv,
                      void (*listfunc)(struct list **),
                      void (*hashfunc)(struct hash **),
                      void (*bitmapfunc)(struct bitmap **, size_t))
{
    int number;
    int size;

    if (same(ins, argv[0]))
    {
        if(argc<3)
            return STATE_NOPARAMETER;

        if (same("list", argv[1]))
        {
            if (samen("list", argv[2]))
            {
                number = getNum("list", argv[2]);
                if (number == NONUMBER)
                    return STATE_NOINT;
                if (number >= 10)
                    return STATE_EXCESSINT;
                listfunc(&(m_list[number]));
                return STATE_COMPLETE;
            }
            return STATE_NOPARAMETER;
        }
        else if (same("hashtable", argv[1]))
        {
                if(samen("hash",argv[2]))
                {
                    number = getNum("hash", argv[2]);
                    if (number == NONUMBER)
                        return STATE_NOINT;
                    if (number >= 10)
                        return STATE_EXCESSINT;
                    hashfunc(&(m_hash[number]));
                    return STATE_COMPLETE;
                }
                return STATE_NOPARAMETER;
        }
        else if (same("bitmap", argv[1]))
        {
            if (argc < 4)
                return STATE_NOPARAMETER;

            if (samen("bm", argv[2]))
            {
                number = getNum("bm", argv[2]);
                size = parseInt(argv[3], 0, strlen(argv[3]) - 1);
                if (number == NONUMBER)
                    return STATE_NOINT;
                if (number >= 10)
                    return STATE_EXCESSINT;
                bitmapfunc(&(m_bitmap[number]), size);
                return STATE_COMPLETE;
            }
            return STATE_NOPARAMETER;
        }
        else
            return STATE_NOPARAMETER;
    }
    return STATE_INCOMPLETE;
}

//The instruction form in shell like
// command data_structure_name
// when directrly revise the data_structur_name
static int template_2(const char *ins, int argc, char **argv,
                      void (*listfunc)(struct list **),
                      void (*hashfunc)(struct hash **),
                      void (*bitmapfunc)(struct bitmap **))
{
    int number;
    if (same(ins, argv[0]))
    {
        if(argc<2)
            return STATE_NOPARAMETER;

        if (samen("list", argv[1]))
        {
            number = getNum("list", argv[1]);
            if (number == NONUMBER)
                return STATE_NOINT;
            if (number >= 10)
                return STATE_EXCESSINT;

            if(m_list[number] == NULL)
                return STATE_DATANULL;

            listfunc(&(m_list[number]));
            return STATE_COMPLETE;
        }
        else if (samen("hash", argv[1]))
        {
            number = getNum("hash", argv[1]);
            if (number == NONUMBER)
                return STATE_NOINT;
            if (number >= 10)
                return STATE_EXCESSINT;

            if(m_hash[number] == NULL)
                return STATE_DATANULL;

            hashfunc(&(m_hash[number]));
            return STATE_COMPLETE;
        }
        else if (samen("bm", argv[1]))
        {

            number = getNum("bm", argv[1]);
            if (number == NONUMBER)
                return STATE_NOINT;
            if (number >= 10)
                return STATE_EXCESSINT;

            if(m_bitmap[number] == NULL)
                return STATE_DATANULL;

            bitmapfunc(&(m_bitmap[number]));
            return STATE_COMPLETE;
        }
        else
            return STATE_NOPARAMETER;
    }
    return STATE_INCOMPLETE;
}

//The instruction form in shell like
// command data_structure_name
// when directrly revise the data_structur_name
static int template_3(const char *ins, int argc, char **argv,
                      void (*listfunc)(struct list *),
                      void (*hashfunc)(struct hash *),
                      void (*bitmapfunc)(const struct bitmap *))
{
    int number;
    if (same(ins, argv[0]))
    {
        if(argc<2)
            return STATE_NOPARAMETER;

        if (samen("list", argv[1]))
        {
            number = getNum("list", argv[1]);
            if (number == NONUMBER)
                return STATE_NOINT;
            if (number >= 10)
                return STATE_EXCESSINT;
            if(m_list[number] == NULL)
                return STATE_DATANULL;
            listfunc((m_list[number]));
            return STATE_COMPLETE;
        }
        else if (samen("hash", argv[1]))
        {
            number = getNum("hash", argv[1]);
            if (number == NONUMBER)
                return STATE_NOINT;
            if (number >= 10)
                return STATE_EXCESSINT;

            if(m_hash[number] == NULL)
                return STATE_DATANULL;
            hashfunc((m_hash[number]));
            return STATE_COMPLETE;
        }
        else if (samen("bm", argv[1]))
        {

            number = getNum("bm", argv[1]);
            if (number == NONUMBER)
                return STATE_NOINT;
            if (number >= 10)
                return STATE_EXCESSINT;

            if(m_bitmap[number] == NULL)
                return STATE_DATANULL;
            bitmapfunc((m_bitmap[number]));
            return STATE_COMPLETE;
        }
        else
            return STATE_NOPARAMETER;
    }
    return STATE_INCOMPLETE;
}

/* instruction_integrated */
int ins_create(int argc, char **argv)
{
    return template_1("create", argc, argv, list_new, hash_new, bitmap_new);
}
int ins_delete(int argc, char **argv)
{
    return template_2("delete", argc, argv, list_delete, hash_terminate, bitmap_delete);
}
int ins_dumpdata(int argc, char **argv)
{
    return template_3("dumpdata", argc, argv, list_print, hash_print, bitmap_dump);
}
int ins_quit(int argc, char **argv)
{
    if (argc == 1 && same("quit", argv[0]))
        return STATE_QUIT;
    return STATE_INCOMPLETE;
}
