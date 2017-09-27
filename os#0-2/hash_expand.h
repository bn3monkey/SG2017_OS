#ifndef __HASH_EXPAND__
#define __HASH_EXPAND__

#include "hash.h"

#define he(a) getHashelem(a) 

typedef struct data_hash
{
	struct hash_elem elem;
	int value;
} DH;



/* expanded hash function */

/* About DH */

//get hash_elem which contains list_elem
struct hash_elem * getHashelem(struct list_elem *elem);

//make new node of data_hash
struct data_hash* newDH(int data);
/* !! Only use after removing from Hash !! */
//delete new node of data_hash whicih contains hash_elem
void deleteDH(DH *d);
//get data_hash which contains hash_elem
struct data_hash* getDH(const struct hash_elem *elem);
//get value of data_hash which contains hash_elem
int getValueDH(const struct hash_elem *elem);
//set value of data_hash which contains hash_elem
struct data_hash* setValueDH(struct hash_elem *elem, int data);
//set random value of data_hash which contains hash_elem
struct data_hash* setRandomDH(struct hash_elem *elem);


/* Print */
//Print the element of hash
void hash_elemprint(struct hash_elem* elem);
//Print the all element of hash
void hash_print(struct hash* hash);


/* constructor, deconstructor */
//Make hash list
void hash_new(struct hash** hash);
void hash_terminate(struct hash** hash);

/* Hash func pointer */
//For comparing hash
bool hash_less_cmp(const struct hash_elem *a, const struct hash_elem *b, void *aux);
//For get hash
unsigned hash_hash(const struct hash_elem *elem, void *aux);
//For free used in hash_action_func 
void hash_action_free(struct hash_elem *elem, void *aux);
//For something used in hash_action_func
void hash_action_square(struct hash_elem *elem, void * aux);
void hash_action_triple(struct hash_elem *elem, void * aux);

// For the Shell Instruction
void ihash_empty(struct hash** hash);
void ihash_size(struct hash** hash);
void ihash_clear(struct hash** hash);

void ihash_insert(struct hash** hash, int value);
void ihash_delete(struct hash** hash, int value);
void ihash_find(struct hash** hash, int value);
void ihash_replace(struct hash** hash, int value);

void ihash_apply(struct hash** hash, char* cmd);

#endif
