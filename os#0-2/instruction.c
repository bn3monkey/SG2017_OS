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
                      void (*bitmapfunc)(struct bitmap *))
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
                //return STATE_DATANULL;
            	return STATE_COMPLETE;

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
                //return STATE_DATANULL;
            	return STATE_COMPLETE;

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
		//return STATE_DATANULL;
		return STATE_COMPLETE;

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
    return template_3("dumpdata", argc, argv, list_print, hash_print, bitmap_print);
}
int ins_quit(int argc, char **argv)
{
    if (argc == 1 && same("quit", argv[0]))
        return STATE_QUIT;
    return STATE_INCOMPLETE;
}

//The Template of list instruction
int list_template_1(const char *ins, int argc, char **argv,
                    void (*listfunc)(struct list **))
{
    int number;
    if (same(ins, argv[0]))
    {
        if (argc < 2)
            return STATE_NOPARAMETER;

        if (samen("list", argv[1]))
        {
            number = getNum("list", argv[1]);
            if (number == NONUMBER)
                return STATE_NOINT;
            if (number >= 10)
                return STATE_EXCESSINT;
            if (m_list[number] == NULL)
                //return STATE_DATANULL;
                return STATE_COMPLETE;

            listfunc(&(m_list[number]));
            return STATE_COMPLETE;
        }
    }
    return STATE_INCOMPLETE;
}

int list_template_2(const char *ins, int argc, char **argv,
                    void (*listfunc)(struct list **, int))
{
    int number, size;
    if (same(ins, argv[0]))
    {
        if (argc < 3)
            return STATE_NOPARAMETER;

        if (samen("list", argv[1]))
        {
            number = getNum("list", argv[1]);
            size = parseInt(argv[2], 0, strlen(argv[2]) - 1);
            if (number == NONUMBER)
                return STATE_NOINT;
            if (number >= 10)
                return STATE_EXCESSINT;
            if (size == NONUMBER)
                return STATE_NOINT;
            if (m_list[number] == NULL)
                //return STATE_DATANULL;
                return STATE_COMPLETE;

            listfunc(&(m_list[number]), size);
            return STATE_COMPLETE;
        }

        return STATE_NOPARAMETER;
    }
    return STATE_INCOMPLETE;
}

int list_template_3(const char *ins, int argc, char **argv,
                    void (*listfunc)(struct list **, struct list **))
{
    int number1, number2;
    if (same(ins, argv[0]))
    {
        if (argc < 2)
            return STATE_NOPARAMETER;

        if (samen("list", argv[1]))
        {
            number1 = getNum("list", argv[1]);
            if (number1 == NONUMBER)
                return STATE_NOINT;
            if (number1 >= 10)
                return STATE_EXCESSINT;
            if (m_list[number1] == NULL)
                //return STATE_DATANULL;
                return STATE_COMPLETE;
            
            if(argc == 2)
            {
                listfunc(&(m_list[number1]), NULL);
                return STATE_COMPLETE;
            }

            if (argc < 3)
                return STATE_NOPARAMETER;

            if (samen("list", argv[2]))
            {
                number2 = getNum("list", argv[2]);
                if (number2 == NONUMBER)
                    return STATE_NOINT;
                if (number2 >= 10)
                    return STATE_EXCESSINT;
                if (m_list[number2] == NULL)
                    //return STATE_DATANULL;
                    return STATE_COMPLETE;
                listfunc(&(m_list[number1]), &(m_list[number2]));
                return STATE_COMPLETE;
            }
            return STATE_NOPARAMETER;
        }
        return STATE_NOPARAMETER;
    }
    return STATE_INCOMPLETE;
}

int list_template_4(const char *ins, int argc, char **argv,
                    void (*listfunc)(struct list **, int, int))
{
    int number, size1, size2;
    if (same(ins, argv[0]))
    {
        if (argc < 4)
            return STATE_NOPARAMETER;

        if (samen("list", argv[1]))
        {
            number = getNum("list", argv[1]);
            size1 = parseInt(argv[2], 0, strlen(argv[2]) - 1);
            size2 = parseInt(argv[3], 0, strlen(argv[3]) - 1);
            if (number == NONUMBER)
                return STATE_NOINT;
            if (number >= 10)
                return STATE_EXCESSINT;
            if (size1 == NONUMBER)
                return STATE_NOINT;
            if (size2 == NONUMBER)
                return STATE_NOINT;
            if (m_list[number] == NULL)
                //return STATE_DATANULL;
                return STATE_COMPLETE;

            listfunc(&(m_list[number]), size1, size2);
            return STATE_COMPLETE;
        }

        return STATE_NOPARAMETER;
    }
    return STATE_INCOMPLETE;
}

int list_template_5(const char *ins, int argc, char **argv,
                    void (*listfunc)(struct list **, int, struct list **, int, int))
{
    int number1, number2, i,j,k;
    if (same(ins, argv[0]))
    {
        if (argc < 6)
            return STATE_NOPARAMETER;

        if (samen("list", argv[1]))
        {
            number1 = getNum("list", argv[1]);
            i = parseInt(argv[2], 0, strlen(argv[2]) - 1);
            
            if (number1 == NONUMBER)
                return STATE_NOINT;
            if (number1 >= 10)
                return STATE_EXCESSINT;
            if (i == NONUMBER)
                return STATE_NOINT;
            if (m_list[number1] == NULL)
                //return STATE_DATANULL;
                return STATE_COMPLETE;

            if(samen("list", argv[3]))
            {
                number2 = getNum("list", argv[3]);
                j = parseInt(argv[4], 0, strlen(argv[4]) - 1);
                k = parseInt(argv[5], 0, strlen(argv[5]) - 1);
                if (number2 == NONUMBER)
                     return STATE_NOINT;
                if (number2 >= 10)
                     return STATE_EXCESSINT;
                if (j == NONUMBER)
                     return STATE_NOINT;
                if (k == NONUMBER)
                     return STATE_NOINT;

                listfunc(&(m_list[number1]), i, &(m_list[number2]),j, k);
                return STATE_COMPLETE;
            }
            return STATE_NOPARAMETER;
        }

        return STATE_NOPARAMETER;
    }
    return STATE_INCOMPLETE;
}

/* instruction_list */
//The Template of list instruction 1

int ins_list_pop_front(int argc, char** argv)
{
    return list_template_1("list_pop_back",argc,argv,ilist_pop_back);
}
int ins_list_pop_end(int argc, char** argv)
{
    return list_template_1("list_pop_front",argc,argv,ilist_pop_front);
}
int ins_list_front(int argc, char** argv)
{
    return list_template_1("list_front",argc,argv,ilist_front);
}
int ins_list_back(int argc, char** argv)
{
    return list_template_1("list_back",argc,argv,ilist_back);
}
int ins_list_size(int argc, char** argv)
{
    return list_template_1("list_size",argc,argv,ilist_size);
}
int ins_list_empty(int argc, char** argv)
{
    return list_template_1("list_empty",argc,argv,ilist_empty);
}
int ins_list_max(int argc, char** argv)
{
    return list_template_1("list_max",argc,argv,ilist_max);
}
int ins_list_min(int argc, char** argv)
{
    return list_template_1("list_min",argc,argv,ilist_min);
}
int ins_list_reverse(int argc, char** argv)
{
    return list_template_1("list_reverse",argc,argv,ilist_reverse);
}
int ins_list_sort(int argc, char** argv)
{
    return list_template_1("list_sort",argc,argv,ilist_sort);
}

int ins_list_shuffle(int argc, char ** argv)
{
    return list_template_1("list_shuffle",argc,argv,ilist_shuffle);
}

//The Template of list instruction 2
int ins_list_push_back(int argc, char** argv)
{
    return list_template_2("list_push_back",argc,argv,ilist_push_back);
}
int ins_list_push_front(int argc, char** argv)
{
    return list_template_2("list_push_front",argc,argv,ilist_push_front);
}
int ins_list_insert_ordered(int argc, char** argv)
{
    return list_template_2("list_insert_ordered",argc,argv,ilist_insert_ordered);
}
int ins_list_remove(int argc, char** argv)
{
    return list_template_2("list_remove",argc,argv,ilist_remove);
}

//The Template of list instruction 3
int ins_list_unique(int argc, char** argv)
{
    return list_template_3("list_unique",argc,argv,ilist_unique);
}

//The Template of list instruction 4
int ins_list_insert(int argc, char** argv)
{
    return list_template_4("list_insert",argc,argv,ilist_insert);
}
int ins_list_swap(int argc, char ** argv)
{
    return list_template_4("list_swap",argc,argv,ilist_swap);
}


//The Template of list instruction 5
int ins_list_splice(int argc, char** argv)
{
    return list_template_5("list_splice",argc,argv,ilist_splice);
}



//The Template of hash instruction
static int hash_template_1(const char *ins, int argc, char **argv,
                    void (*hashfunc)(struct hash **))
{
    int number;
    if (same(ins, argv[0]))
    {
        if (argc < 2)
            return STATE_NOPARAMETER;

        if (samen("hash", argv[1]))
        {
            number = getNum("hash", argv[1]);
            if (number == NONUMBER)
                return STATE_NOINT;
            if (number >= 10)
                return STATE_EXCESSINT;
            if (m_hash[number] == NULL)
                //return STATE_DATANULL;
                return STATE_COMPLETE;

                hashfunc(&(m_hash[number]));
            return STATE_COMPLETE;
        }
    }
    return STATE_INCOMPLETE;
}

static int hash_template_2(const char *ins, int argc, char **argv,
                    void (*hashfunc)(struct hash **, int))
{
    int number, size;
    if (same(ins, argv[0]))
    {
        if (argc < 3)
            return STATE_NOPARAMETER;

        if (samen("hash", argv[1]))
        {
            number = getNum("hash", argv[1]);
            size = parseInt(argv[2], 0, strlen(argv[2]) - 1);
            if (number == NONUMBER)
                return STATE_NOINT;
            if (number >= 10)
                return STATE_EXCESSINT;
            if (size == NONUMBER)
                return STATE_NOINT;
            if (m_hash[number] == NULL)
                //return STATE_DATANULL;
                return STATE_COMPLETE;

            hashfunc(&(m_hash[number]), size);
            return STATE_COMPLETE;
        }

        return STATE_NOPARAMETER;
    }
    return STATE_INCOMPLETE;
}

static int hash_template_3(const char *ins, int argc, char **argv,
                    void (*hashfunc)(struct hash **, char *))
{
    int number1;
    if (same(ins, argv[0]))
    {
        if (argc < 3)
            return STATE_NOPARAMETER;

        if (samen("hash", argv[1]))
        {
            number1 = getNum("hash", argv[1]);
            if (number1 == NONUMBER)
                return STATE_NOINT;
            if (number1 >= 10)
                return STATE_EXCESSINT;
            if (m_hash[number1] == NULL)
                //return STATE_DATANULL;
                return STATE_COMPLETE;

            hashfunc(&(m_hash[number1]), argv[2]);
            return STATE_COMPLETE;
            
            return STATE_NOPARAMETER;
        }
        return STATE_NOPARAMETER;
    }
    return STATE_INCOMPLETE;
}

int ins_hash_empty(int argc, char ** argv)
{
    return hash_template_1("hash_empty", argc, argv, ihash_empty);
}
int ins_hash_size(int argc, char** argv)
{
    return hash_template_1("hash_size", argc, argv, ihash_size);
}
int ins_hash_clear(int argc, char** argv)
{
    return hash_template_1("hash_clear", argc, argv, ihash_clear);
}

int ins_hash_insert(int argc, char ** argv)
{
    return hash_template_2("hash_insert", argc, argv, ihash_insert);
}
int ins_hash_delete(int argc, char** argv)
{
    return hash_template_2("hash_delete", argc, argv, ihash_delete);
}
int ins_hash_find(int argc, char** argv)
{
    return hash_template_2("hash_find", argc, argv, ihash_find);
}
int ins_hash_replace(int argc, char** argv)
{
    return hash_template_2("hash_replace", argc, argv, ihash_replace);
}

int ins_hash_apply(int argc, char** argv)
{
    return hash_template_3("hash_apply", argc, argv, ihash_apply);    
}

//The Template of bitmap instruction
static int bitmap_template_1(const char *ins, int argc, char **argv,
                           void (*bitmapfunc)(struct bitmap **))
{
    int number;
    if (same(ins, argv[0]))
    {
        if (argc < 2)
            return STATE_NOPARAMETER;

        if (samen("bm", argv[1]))
        {
            number = getNum("bm", argv[1]);
            if (number == NONUMBER)
                return STATE_NOINT;
            if (number >= 10)
                return STATE_EXCESSINT;
            if (m_bitmap[number] == NULL)
                //return STATE_DATANULL;
                return STATE_COMPLETE;

            bitmapfunc(&(m_bitmap[number]));
            return STATE_COMPLETE;
        }
    }
    return STATE_INCOMPLETE;
}

static int bitmap_template_2(const char *ins, int argc, char **argv,
                           void (*bitmapfunc)(struct bitmap **, int))
{
    int number, size;
    if (same(ins, argv[0]))
    {
        if (argc < 3)
            return STATE_NOPARAMETER;

        if (samen("bm", argv[1]))
        {
            number = getNum("bm", argv[1]);
            size = parseInt(argv[2], 0, strlen(argv[2]) - 1);
            if (number == NONUMBER)
                return STATE_NOINT;
            if (number >= 10)
                return STATE_EXCESSINT;
            if (size == NONUMBER)
                return STATE_NOINT;
            if (m_bitmap[number] == NULL)
                //return STATE_DATANULL;
                return STATE_COMPLETE;

            bitmapfunc(&(m_bitmap[number]), size);
            return STATE_COMPLETE;
        }

        return STATE_NOPARAMETER;
    }
    return STATE_INCOMPLETE;
}

static int bitmap_template_3(const char *ins, int argc, char **argv,
                           void (*bitmapfunc)(struct bitmap **, char *))
{
    int number1;
    if (same(ins, argv[0]))
    {
        if (argc < 3)
            return STATE_NOPARAMETER;

        if (samen("bm", argv[1]))
        {
            number1 = getNum("bm", argv[1]);
            if (number1 == NONUMBER)
                return STATE_NOINT;
            if (number1 >= 10)
                return STATE_EXCESSINT;
            if (m_bitmap[number1] == NULL)
                //return STATE_DATANULL;
                return STATE_COMPLETE;

            bitmapfunc(&(m_bitmap[number1]), argv[2]);
            return STATE_COMPLETE;

            return STATE_NOPARAMETER;
        }
        return STATE_NOPARAMETER;
    }
    return STATE_INCOMPLETE;
}

static int bitmap_template_4(const char *ins, int argc, char **argv,
                             void (*bitmapfunc)(struct bitmap **, int, int))
{
    int number, size1, size2;
    if (same(ins, argv[0]))
    {
        if (argc < 4)
            return STATE_NOPARAMETER;

        if (samen("bm", argv[1]))
        {
            number = getNum("bm", argv[1]);
            size1 = parseInt(argv[2], 0, strlen(argv[2]) - 1);
            size2 = parseInt(argv[3], 0, strlen(argv[3]) - 1);
            if (number == NONUMBER)
                return STATE_NOINT;
            if (number >= 10)
                return STATE_EXCESSINT;
            if (size1 == NONUMBER)
                return STATE_NOINT;
            if (size2 == NONUMBER)
                return STATE_NOINT;
            if (m_bitmap[number] == NULL)
                //return STATE_DATANULL;
                return STATE_COMPLETE;

            bitmapfunc(&(m_bitmap[number]), size1, size2);
            return STATE_COMPLETE;
        }

        return STATE_NOPARAMETER;
    }
    return STATE_INCOMPLETE;
}

static int bitmap_template_5(const char *ins, int argc, char **argv,
                             void (*bitmapfunc)(struct bitmap **, int, char*))
{
    int number, size1;
    if (same(ins, argv[0]))
    {
        if (argc < 4)
            return STATE_NOPARAMETER;

        if (samen("bm", argv[1]))
        {
            number = getNum("bm", argv[1]);
            size1 = parseInt(argv[2], 0, strlen(argv[2]) - 1);
            if (number == NONUMBER)
                return STATE_NOINT;
            if (number >= 10)
                return STATE_EXCESSINT;
            if (size1 == NONUMBER)
                return STATE_NOINT;
            if (m_bitmap[number] == NULL)
                //return STATE_DATANULL;
                return STATE_COMPLETE;

            bitmapfunc(&(m_bitmap[number]), size1, argv[3]);
            return STATE_COMPLETE;
        }

        return STATE_NOPARAMETER;
    }
    return STATE_INCOMPLETE;
}

static int bitmap_template_6(const char *ins, int argc, char **argv,
                             void (*bitmapfunc)(struct bitmap **, int, int, char*))
{
    int number, size1, size2;
    if (same(ins, argv[0]))
    {
        if (argc < 5)
            return STATE_NOPARAMETER;

        if (samen("bm", argv[1]))
        {
            number = getNum("bm", argv[1]);
            size1 = parseInt(argv[2], 0, strlen(argv[2]) - 1);
            size2 = parseInt(argv[3], 0, strlen(argv[3]) - 1);
            if (number == NONUMBER)
                return STATE_NOINT;
            if (number >= 10)
                return STATE_EXCESSINT;
            if (size1 == NONUMBER)
                return STATE_NOINT;
            if (size2 == NONUMBER)
                return STATE_NOINT;
            if (m_bitmap[number] == NULL)
                //return STATE_DATANULL;
                return STATE_COMPLETE;

            bitmapfunc(&(m_bitmap[number]), size1, size2, argv[4]);
            return STATE_COMPLETE;
        }

        return STATE_NOPARAMETER;
    }
    return STATE_INCOMPLETE;
}

int ins_bitmap_dump(int argc, char** argv)
{
    return bitmap_template_1("bitmap_dump", argc, argv,ibitmap_dump);
}
int ins_bitmap_size(int argc, char** argv)
{
    return bitmap_template_1("bitmap_size", argc, argv,ibitmap_size);    
}
int ins_bitmap_mark(int argc, char** argv)
{
    return bitmap_template_2("bitmap_mark", argc, argv, ibitmap_mark);
}
int ins_bitmap_expand(int argc, char** argv)
{
    return bitmap_template_2("bitmap_expand", argc, argv,ibitmap_expand);    
}
int ins_bitmap_flip(int argc, char** argv)
{
    return bitmap_template_2("bitmap_flip", argc, argv,ibitmap_flip);     
}
int ins_bitmap_reset(int argc, char** argv)
{
    return bitmap_template_2("bitmap_reset", argc, argv,ibitmap_reset);     
}
int ins_bitmap_test(int argc, char** argv)
{
    return bitmap_template_2("bitmap_test", argc, argv,ibitmap_test);     
}


int ins_bitmap_set_all(int argc, char** argv)
{
    return bitmap_template_3("bitmap_set_all", argc, argv, ibitmap_set_all);   
}


int ins_bitmap_all(int argc, char** argv)
{
    return bitmap_template_4("bitmap_all", argc, argv,ibitmap_all);    
}
int ins_bitmap_any(int argc, char** argv)
{
    return bitmap_template_4("bitmap_any", argc, argv,ibitmap_any);  
}
int ins_bitmap_none(int argc, char** argv)
{
    return bitmap_template_4("bitmap_none", argc, argv,ibitmap_none);     
}

int ins_bitmap_set(int argc, char** argv)
{
    return bitmap_template_5("bitmap_set", argc, argv,ibitmap_set);      
}

int ins_bitmap_contains(int argc, char** argv)
{
    return bitmap_template_6("bitmap_contains", argc, argv, ibitmap_contains);      
}
int ins_bitmap_count(int argc, char** argv)
{
    return bitmap_template_6("bitmap_count", argc, argv, ibitmap_count);       
}
int ins_bitmap_scan(int argc, char** argv)
{
    return bitmap_template_6("bitmap_scan", argc, argv, ibitmap_scan);       
}
int ins_bitmap_scan_and_flip(int argc, char** argv)
{
    return bitmap_template_6("bitmap_scan_and_flip", argc, argv, ibitmap_scan_and_flip);     
}
int ins_bitmap_set_multiple(int argc, char** argv)
{
    return bitmap_template_6("bitmap_set_multiple", argc, argv, ibitmap_set_multiple); 
}

