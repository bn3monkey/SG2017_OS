#include "list_expand.h"
#include <assert.h>
#include <stdio.h>
#define ASSERT(CONDITION) assert(CONDITION)

/* Returns true if ELEM is an interior element,
   false otherwise. */
static inline bool
is_interior (struct list_elem *elem)
{
  return elem != NULL && elem->prev != NULL && elem->next != NULL;
}



//make new node of data_listed
struct data_listed* newDL(int data)
{
	DL* temp = (DL *)malloc(sizeof(DL));
	temp->elem.prev = NULL;
	temp->elem.next = NULL;
	temp->value = data;
	return temp;
}

/* !! Only use after removing from List !! */
//delete new node of data listed whicih contains list_elem
void deleteDL(struct list_elem *elem) 
{
	ASSERT (elem!=NULL);
	free(getDL(elem));	
}
//get data_listed which contains list_elem
struct data_listed* getDL(struct list_elem *elem)
{
	ASSERT (elem!=NULL);
	DL* temp = list_entry(elem, struct data_listed ,elem);
	return temp;	
	
}
//get value of data_listed which contains list_elem
int getValueDL(struct list_elem *elem)
{
	ASSERT(elem!=NULL);
	DL* temp = getDL(elem);
	return temp->value;
}
//set value of data_listed which contains list_elem
struct data_listed* setValueDL(struct list_elem *elem, int data)
{
	ASSERT(elem!=NULL);
	DL* temp = getDL(elem);
	temp->value = data;
	return temp;
}
//set random value of data_listed which contains list elem
struct data_listed* setRandomDL(struct list_elem *elem)
{
	ASSERT(elem!=NULL);
	DL* temp = setValueDL(elem, rand() % 5000);
	return temp;
}


//Find the element of list
struct list_elem* list_find(struct list* list, int data)
{
	ASSERT(list!=NULL);
	ASSERT(!(list_empty(list)));
	struct list_elem* ptr;
	struct list_elem* end;
	end = list_end(list);
	for(ptr = list_begin(list);ptr!=end;ptr = list_next(ptr))
	{
		if(getValueDL(ptr) == data)
			return ptr;	
	}
	return NULL;
}

//free all element of list
void list_clean(struct list* list)
{
	ASSERT(list!=NULL);

	while(!(list_empty(list)))
	{
		deleteDL(list_pop_front(list));
	}
	return;
}

//Print the element of list
void list_elemprint(struct list_elem* elem)
{	
	printf("%d ",getValueDL(elem));
}
//Print the all element of list
void list_print(struct list* list)
{
	ASSERT(list!=NULL);
	if(list_empty(list))
	{
		printf("List Empty\n");
		return;
	}
	struct list_elem* ptr;
	struct list_elem* end;
	end = list_end(list);
	for(ptr = list_begin(list);ptr!=end;ptr = list_next(ptr))
	{
		list_elemprint(ptr);
	}
	printf("\n");
}


//Make struct list
void list_new(struct list** list)
{
	ASSERT(*list==NULL);
	
	if(*list!=NULL)
		list_delete(list);
	
	*list = (struct list *)malloc(sizeof(struct list));
	list_init(*list);
	return;
}

//Free struct list
void list_delete(struct list** list)
{
	ASSERT(*list!=NULL);
	list_clean(*list);

	free(*list);
	*list = NULL;
}

bool list_less_cmp(struct list_elem *a, struct list_elem *b)
{
	return (getValueDL(a) < getValueDL(b));
}



