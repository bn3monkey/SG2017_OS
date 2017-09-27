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

//Find the element of list which corresponding to list
struct list_elem* list_findi(struct list* list, int index)
{
	ASSERT(list!=NULL);
	ASSERT(!(list_empty(list)));
	struct list_elem* ptr;
	struct list_elem* end;
	end = list_end(list);
	for(ptr = list_begin(list);ptr!=end;ptr = list_next(ptr))
	{
		if(index--)
			return ptr;	
	}
	return NULL;
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

bool list_less_cmp(const struct list_elem *a, const struct list_elem *b, void* AUX)
{
	ASSERT (a!=NULL && b!=NULL);
	DL* x = list_entry(a, struct data_listed ,elem);
	DL* y = list_entry(b, struct data_listed ,elem);

	return (x->value < y->value);
}


/* For the Shell Instruction */

//list_instruction_1
void ilist_pop_back(struct list** list)
{
	struct list_elem* temp = list_pop_back((*list));
	deleteDL(temp);
}
void ilist_pop_front(struct list** list)
{
	struct list_elem* temp = list_pop_front((*list));
	deleteDL(temp);	
}
void ilist_front(struct list** list)
{
	struct list_elem* temp = list_front((*list));
	list_elemprint(temp);
	printf("\n");	
}
void ilist_back(struct list** list)
{
	struct list_elem* temp = list_back((*list));
	list_elemprint(temp);
	printf("\n");
}
void ilist_size(struct list** list)
{
	printf("%d",(int)list_size((*list)));
	printf("\n");
}
void ilist_empty(struct list** list)
{
	printf("%s", list_empty((*list)) ? "true":"false");
	printf("\n");
}
void ilist_max(struct list** list)
{
	struct list_elem* temp = list_max(*list, list_less_cmp, NULL);
	printf("%d", getValueDL(temp));
	printf("\n");
}
void ilist_min(struct list** list)
{
	struct list_elem* temp = list_min(*list, list_less_cmp, NULL);
	printf("%d", getValueDL(temp));
	printf("\n");
}
void ilist_reverse(struct list** list)
{
	list_reverse(*list);
}
void ilist_sort(struct list** list)
{
	list_sort(*list, list_less_cmp, NULL);
}

//list_instruction_2
void ilist_push_back(struct list** list,int value)
{
	DL* temp = newDL(value);
	list_push_back(*list,&(temp->elem));
}
void ilist_push_front(struct list** list,int value)
{
	DL* temp = newDL(value);
	list_push_front(*list,&(temp->elem));
}
void ilist_insert_ordered(struct list** list,int value)
{
	DL* temp = newDL(value);
	list_insert_ordered(*list, &(temp->elem), list_less_cmp, NULL); 
}
void ilist_remove(struct list** list, int index)
{
	struct list_elem* temp = list_findi(*list, index);
	temp = list_pop(temp);
	deleteDL(temp);
}
//list_instruction_3
void ilist_unique(struct list** dest, struct list** sour)
{
	list_unique(*sour, *dest, list_less_cmp, NULL);
}

//list_instruction_4
void ilist_insert(struct list** list, int index, int value)
{
	struct list_elem* temp = list_findi(*list, index);
	temp = temp->prev;
	DL* inserted = newDL(value);
	list_insert(temp, &(inserted->elem));
}

//list_instruction_5
void ilist_splice(struct list** dest, int index, struct list** sour, int start, int end)
{
	struct list_elem* before = list_findi(*dest, index);
	before = before->prev;
	struct list_elem* first = list_findi(*sour, start);
	struct list_elem* last = list_findi(*sour, end);

	list_splice(before, first, last);
}

