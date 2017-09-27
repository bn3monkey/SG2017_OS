#ifndef __LIST__EXPAND__
#define __LIST__EXPAND__

#include "list.h"

typedef struct data_listed
{
	struct list_elem elem;
	int value; 
} DL;

/* About DL */

//make new node of data_listed
struct data_listed* newDL(int data);
/* !! Only use after removing from List !! */
//delete new node of data listed whicih contains list_elem
void deleteDL(struct list_elem *elem); 
//get data_listed which contains list_elem
struct data_listed* getDL(struct list_elem *elem);
//get value of data_listed which contains list_elem
int getValueDL(struct list_elem *elem);
//set value of data_listed which contains list_elem
struct data_listed* setValueDL(struct list_elem *elem, int data);
//set random value of data_listed which contains list elem
struct data_listed* setRandomDL(struct list_elem *elem);



/* Expanded list function*/


/* Getting Element */
//Find the element of list which corresponding to list
struct list_elem* list_findi(struct list* list, int index); 
//Find the element of list 
struct list_elem* list_find(struct list* list, int data);

/* Free Data Cluster */
//free all element of list
void list_clean(struct list* list);

/* Print */
//Print the element of list
void list_elemprint(struct list_elem* elem);
//Print the all element of list
void list_print(struct list* list);

/* constructor, deconstructor */
//Free struct list
void list_delete(struct list** list);
//Make struct list
void list_new(struct list** list);

//comperator
bool list_less_cmp(struct list_elem *a, struct list_elem *b);

/* For the Shell Instruction */

//list_instruction_1
void ilist_pop_back(struct list** list);
void ilist_pop_front(struct list** list);
void ilist_front(struct list** list);
void ilist_back(struct list** list);
void ilist_size(struct list** list);
void ilist_empty(struct list** list);
void ilist_max(struct list** list);
void ilist_min(struct list** list);
void ilist_reverse(struct list** list);
void ilist_sort(struct list** list);

//list_instruction_2
void ilist_push_back(struct list** list,int value);
void ilist_push_front(struct list** list,int value);
void ilist_insert_ordered(struct list** list,int value);
void ilist_remove(struct list** list, int index);

//list_instruction_3
void ilist_unique(struct list** dest, struct list** sour);

//list_instruction_4
void ilist_insert(struct list** list, int index, int value);

//list_instruction_5
void ilist_splice(struct list** dest, int index, struct list** sour, int start, int end);
#endif
