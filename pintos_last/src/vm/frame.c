#include "vm/frame.h"
#include "threads/palloc.h"
#include "threads/malloc.h"

/* frame_table 관련 함수 */
//frame table 초기화
bool init_frame_table(void)
{
    list_init(&frame_table);
    lock_init(&frame_table_lock);
    return true;
}
//frame table 파괴
bool destroy_frame_table(void)
{
    struct list_elem* elem;
    struct frame_entry* entry;
    while(list_empty(&frame_table))
    {
        //list에서 빠지는 부분만, 동기화한다.
        lock_acquire(&frame_table_lock);
        elem = list_pop_back(&frame_table);
        lock_release(&frame_table_lock);

        entry = elem_to_frame_entry(elem);
        palloc_free_page(entry->paddr);
        free(entry);
    }
    return true;
}

/* frame_entry 관련 함수 */
//frame으로 초기화된 frame_entry를 리턴한다.
struct frame_entry* init_frame_entry(void* paddr, struct page_entry * pte)
{
    struct frame_entry* entry = (struct frame_entry *)malloc(sizeof(struct frame_entry));
    entry->paddr = paddr;
    entry->pte = pte;
    return entry;
}

//원자화를 유지한 채, entry를 frame_table에 잇는다.
static void add_to_frame_table(struct frame_entry* entry)
{
    lock_acquire(&frame_table_lock);
    list_push_back(&frame_table, &(entry->elem));
    lock_release(&frame_table_lock);
}

//frame_entry를 table에 추가한다. (physical memory에 할당!)
void* set_frame_entry(struct page_entry* pte)
{
    void* paddr;
    struct frame_entry* entry;
    paddr = palloc_get_page(PAL_USER);

    //1. Free인 frame이 존재하지 않으면,  evict를 수행한다.
    if(!paddr)
    {
        paddr = evict_frame();
        //2. evict_frame을 수행했음에도, paddr == NULL이면
        //SWAP_DISK도 꽉 찼기 때문에 NULL을 return한다.
        //나중에 page_fault에서 NULL 받으면 KILL내거나 하면 됨
        if(!paddr)
        {
            //Project 3 : Suggestion 1
            PANIC("Frame space is FULL!");
            return NULL;
        }
    }
    entry = init_frame_entry(paddr, pte);
    add_to_frame_table(entry);
    return paddr;
}
//frame_entry를 table에서 뺀다. (physical memory에서 해제!)
bool delete_frame_entry(void* paddr)
{
    struct list_elem* elem;
    struct frame_entry* entry;

    for(elem = list_begin(&frame_table);
        elem != list_end(&frame_table);
        elem = list_next(elem))
        {
            entry = elem_to_frame_entry(elem);
            if(entry->paddr == paddr)
            {
                lock_acquire(&frame_table_lock);
                list_remove(elem);
                lock_release(&frame_table_lock);

                palloc_free_page(entry->paddr);
                free(entry);
                return true;
            }
        }
    //해당 frame이 없음
    return false;
}

//elem에서 frame_entry를 가져온다.
struct frame_entry* elem_to_frame_entry(struct list_elem* elem)
{
    return list_entry(elem, struct frame_entry, elem);
}

//frame에서 page를 가져온다.
struct page_entry* frame_to_page(struct frame_entry* entry)
{
    return entry->pte;
}


//더 이상 할당할 frame이 없는 경우, evict할 frame을 찾아서 위치를 리턴한다.
void* evict_frame (void)
{
    return NULL;
}
