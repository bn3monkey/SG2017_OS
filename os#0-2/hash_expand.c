#include "hash_expand.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define ASSERT(CONDITION) assert(CONDITION)

#define DEBUG

/* expanded hash function */

/* About DH */

//get hash_elem which contains list_elem
struct hash_elem * getHashelem(struct list_elem *elem)
{
	ASSERT(elem!=NULL);
	return list_entry(elem, struct hash_elem, list_elem);
}
//make new node of data_hash
struct data_hash* newDH(int data)
{
	DH *temp = (DH *)malloc(sizeof(DH));
	temp->value = data;
	temp->elem.list_elem.prev = NULL;
	temp->elem.list_elem.next = NULL;
	return temp;
}
/* !! Only use after removing from Hash !! */
//delete new node of data_hash whicih contains hash_elem
void deleteDH(struct hash_elem *elem)
{
	ASSERT(elem!=NULL);
	hash_action_free(elem, NULL);
	return;
} 
//get data_hash which contains hash_elem
struct data_hash* getDH(const struct hash_elem *elem)
{
	ASSERT(elem!=NULL);
	DH* temp = hash_entry(elem, struct data_hash, elem);
	return temp;
}
//get value of data_hash which contains hash_elem
int getValueDH(const struct hash_elem *elem)
{
	ASSERT(elem!=NULL);
	DH* temp = getDH(elem);
	return temp->value;
}
//set value of data_hash which contains hash_elem
struct data_hash* setValueDH(struct hash_elem *elem, int data)
{
	ASSERT(elem!=NULL);
	DH* temp = getDH(elem);
	temp->value = data;
	return temp;
}
//set random value of data_hash which contains hash_elem
struct data_hash* setRandomDH(struct hash_elem *elem)
{
	ASSERT(elem!=NULL);
	DH* temp = setValueDH(elem, rand() % 5000);
	return temp;
}

/* Print */
//Print the element of hash
void hash_elemprint(struct hash_elem* elem)
{
	ASSERT(elem!=NULL);
	DH* temp = getDH(elem);
	printf("%d ",temp->value);
}
//Print the all element of hash
void hash_print(struct hash* hash)
{
	int i;
	
	  for (i = 0; i < hash->bucket_cnt; i++) 
		{
		  struct list *bucket = &hash->buckets[i];
	
		  ASSERT(bucket!=NULL);
		  if(list_empty(bucket))
			  continue;
		  struct list_elem* ptr;
		  struct list_elem* end;
		  end = list_end(bucket);
		  for(ptr = list_begin(bucket);ptr!=end;ptr = list_next(ptr))
		  {
			  hash_elemprint(he(ptr));
		  }
		  printf("\n");
		}    
	
}


/* constructor, deconstructor */
//Make hash list
void hash_new(struct hash** hash)
{
	ASSERT(*hash==NULL);
	
	*hash = (struct hash *)malloc(sizeof(struct hash));
	hash_init(*hash, hash_hash, hash_less_cmp, NULL);
}
void hash_terminate(struct hash** hash)
{
	ASSERT(*hash!=NULL);
	hash_destroy(*hash, hash_action_free);
	free(*hash);
	*hash = NULL;
}
/* Hash func pointer */
//For comparing hash
bool hash_less_cmp(const struct hash_elem *a, const struct hash_elem *b, void *aux)
{
	return (getValueDH(a) < getValueDH(b));
}
//For get hash
unsigned hash_hash(const struct hash_elem *elem, void *aux)
{
	DH* temp = getDH(elem);
	return hash_int_2(temp->value);
}
//For free used in hash_action_func 
void hash_action_free(struct hash_elem *elem, void *aux)
{
	ASSERT(elem!=NULL);
	DH *temp = hash_entry(elem, struct data_hash, elem);
	free(temp);
	return;
}
//For something used in hash_action_func
void hash_action_something(struct hash_elem *elem, void * aux)
{
	return;
}
