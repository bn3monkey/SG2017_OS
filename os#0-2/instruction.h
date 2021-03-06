#include "list.h"
#include "hash.h"
#include "bitmap.h"

#include "list_expand.h"
#include "hash_expand.h"

#include "parse.h"



#define same(A,B) strcmp((A),(B))==0
#define samen(A,B) strncmp((A),(B),strlen((A)))==0

#define STATE_COMPLETE 1
#define STATE_INCOMPLETE 0
#define STATE_NONSTRUCTION -201
#define STATE_NOPARAMETER -202
#define STATE_NOINT -203
#define STATE_EXCESSINT -204
#define STATE_DATANULL -205
#define STATE_QUIT -100

extern struct list* m_list[10];
extern struct hash* m_hash[10];
extern struct bitmap* m_bitmap[10];

/* instruction_integrated */
int ins_create(int argc, char** argv);
int ins_delete(int argc, char** argv);
int ins_dumpdata(int argc, char** argv);
int ins_quit(int argc, char** argv);


/* instruction_list */
int ins_list_pop_front(int argc, char** argv);
int ins_list_pop_end(int argc, char** argv);
int ins_list_front(int argc, char** argv);
int ins_list_back(int argc, char** argv);
int ins_list_size(int argc, char** argv);
int ins_list_empty(int argc, char** argv);
int ins_list_max(int argc, char** argv);
int ins_list_min(int argc, char** argv);
int ins_list_reverse(int argc, char** argv);
int ins_list_sort(int argc, char** argv);

int ins_list_push_back(int argc, char** argv);
int ins_list_push_front(int argc, char** argv);
int ins_list_insert_ordered(int argc, char** argv);
int ins_list_remove(int argc, char** argv);

int ins_list_unique(int argc, char** argv);

int ins_list_insert(int argc, char** argv);
int ins_list_swap(int argc, char ** argv);

int ins_list_splice(int argc, char** argv);

int ins_list_shuffle(int argc, char ** argv);

/* instruction_hash */
int ins_hash_empty(int argc, char ** argv);
int ins_hash_size(int argc, char** argv);
int ins_hash_clear(int argc, char** argv);

int ins_hash_insert(int argc, char ** argv);
int ins_hash_delete(int argc, char** argv);
int ins_hash_find(int argc, char** argv);
int ins_hash_replace(int argc, char** argv);

int ins_hash_apply(int argc, char** argv);

/* instruction_bitmap */
int ins_bitmap_dump(int argc, char** argv);
int ins_bitmap_size(int argc, char** argv);


int ins_bitmap_set_all(int argc, char** argv);

int ins_bitmap_mark(int argc, char** argv);
int ins_bitmap_expand(int argc, char** argv);
int ins_bitmap_flip(int argc, char** argv);
int ins_bitmap_reset(int argc, char** argv);
int ins_bitmap_test(int argc, char** argv);

int ins_bitmap_all(int argc, char** argv);
int ins_bitmap_any(int argc, char** argv);
int ins_bitmap_none(int argc, char** argv);

int ins_bitmap_set(int argc, char** argv);

int ins_bitmap_contains(int argc, char** argv);
int ins_bitmap_count(int argc, char** argv);
int ins_bitmap_scan(int argc, char** argv);
int ins_bitmap_scan_and_flip(int argc, char** argv);
int ins_bitmap_set_multiple(int argc, char** argv);

