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

#endif
