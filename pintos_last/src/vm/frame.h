#ifndef __VM_FRAME__
#define __VM_FRAME__


#include "threads/palloc.h"
#include "threads/synch.h"
#include "vm/page.h"
#include <stdbool.h>
#include <stdint.h>
#include <list.h>


//전체 user address들의 physical address(frame)들을 담은 list
struct list frame_table;
//frame_table에 추가 및 삭제 시 동기화를 위한 변수.
struct lock frame_table_lock;

//frame_talbe의 entry
struct frame_entry
{
    void* paddr; //메모리에 올라와있는 실제 주소

    struct page_entry* pte; // frame에 해당하는 page_entry

    struct list_elem elem; //table elem
};

/* frame_table 관련 함수 */
//frame table 초기화
bool init_frame_table(void);
//frame table 파괴
bool destroy_frame_table(void);

/* frame_entry 관련 함수 */
//frame으로 초기화된 frame_entry를 리턴한다.
struct frame_entry* init_frame_entry(void* paddr, struct page_entry* pte);
//frame_entry를 table에 추가한다. (physical memory에 할당!)
void* set_frame_entry(struct page_entry* pte);
//frame_entry를 table에서 뺀다. (physical memory에서 해제!)
bool delete_frame_entry(void* paddr);
//elem에서 frame_entry를 가져온다.
struct frame_entry* elem_to_frame_entry(struct list_elem* elem);

//frame entry에서 page entry를 가져온다.
struct page_entry* frame_to_page(struct frame_entry* entry);


//더 이상 할당할 frame이 없는 경우, evict할 frame을 찾아서 위치를 리턴한다.
void* evict_frame (void);
#endif

