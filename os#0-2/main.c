#include "main.h"

int main()
{

	Cycle();

	return 0;
}

/*
	int i;
	DL* dltemp;
	DH* dhtemp;


	srand(time(NULL));

	list_new(&(m_list[0]));
	for(i=0;i<10;i++)
	{
		dltemp = newDL(0);
		setRandomDL(&(dltemp->elem));
		list_push_back(m_list[0],&(dltemp->elem));
		list_print(m_list[0]);		
	}
	list_swap(list_front(m_list[0]), list_next(list_front(m_list[0])));
	list_print(m_list[0]);	
	list_swap(list_front(m_list[0]), list_next(list_next(list_front(m_list[0]))));
	list_print(m_list[0]);	
	list_print(m_list[0]);	


	
	for(i=0;i<5;i++)
	{	
		printf("\nShuffled %d\n",i);
		list_shuffle(m_list[0]);
		list_print(m_list[0]);
	}

	list_clean(m_list[0]);
	list_print(m_list[0]);
	
	list_delete(&m_list[0]);



	hash_new(&(m_hash[0]));
	for(i=0;i<10;i++)
	{
		dhtemp = newDH(0);
		setRandomDH(&(dhtemp->elem));
		hash_insert(m_hash[0],&(dhtemp->elem));
		hash_print(m_hash[0]);		
	}


	hash_clear(m_hash[0], hash_action_free);
	hash_print(m_hash[0]);
	
	hash_terminate(&m_hash[0]);
	*/