#include "hash_expand.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
void deleteDH(DH *d)
{
	//ASSERT( d != NULL );
	if(d == NULL)
		return;
	hash_action_free(&(d->elem), NULL);
	return;
} 
//get data_hash which contains hash_elem
struct data_hash* getDH(const struct hash_elem *elem)
{
	//ASSERT(elem!=NULL);
	if(elem == NULL)
		return NULL;
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
	DH* temp = getDH((const struct hash_elem *)elem);
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
	
	if(hash_empty(hash))
	{
		return;
	}
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

		}    
		printf("\n");	
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
	//return hash_int_2(temp->value);
	return hash_int(temp->value);
}
//For free used in hash_action_func 
void hash_action_free(struct hash_elem *elem, void *aux)
{
	if(elem==NULL) return;
	DH *temp = hash_entry(elem, struct data_hash, elem);
	free(temp);
	return;
}
//For something used in hash_action_func
void hash_action_square(struct hash_elem *elem, void * aux)
{
	int value = getValueDH((const struct hash_elem *)elem);
	setValueDH(elem, value*value);
	return;
}
void hash_action_triple(struct hash_elem *elem, void * aux)
{
	int value = getValueDH((const struct hash_elem *)elem);
	setValueDH(elem, value*value*value);
	return;
}


// For the Shell Instruction
void ihash_empty(struct hash** hash)
{
	printf("%s\n", hash_empty(*hash)? "true" : "false" );
}
void ihash_size(struct hash** hash)
{
	printf("%d\n", (int)hash_size(*hash));
}
void ihash_clear(struct hash** hash)
{
	hash_clear(*hash, hash_action_free);
}
void ihash_insert(struct hash** hash, int value)
{
	DH* temp = newDH(value);
	hash_insert(*hash, &(temp->elem));
}
void ihash_delete(struct hash** hash, int value)
{
	DH* finder, *temp;
	finder = newDH(value); 
	temp = getDH(hash_delete(*hash, &(finder->elem)));
	deleteDH(temp);
	deleteDH(finder);
}
void ihash_find(struct hash** hash, int value)
{
	DH* finder, *temp;
	finder = newDH(value); 
	temp = getDH(hash_find(*hash, &(finder->elem)));
	if(temp == NULL)
		return;
	printf("%d\n",temp->value);
	deleteDH(finder);
}
void ihash_replace(struct hash** hash, int value)
{
	DH* temp = newDH(value);
	hash_replace(*hash, &(temp->elem));
}
void ihash_apply(struct hash** hash, char* cmd)
{
	if(strcmp(cmd, "square") == 0)
	{
		hash_apply(*hash, hash_action_square);			
	}
	else if(strcmp(cmd, "triple") == 0)
	{
		hash_apply(*hash, hash_action_triple);		
	}
}