#ifndef __VM_PAGE__
#define __VM_PAGE__

#include <hash.h>

#define MAX_STACK (1<<23)

//struct thread 내에 struct hash pt 구조체가 있다.
//thread마다 thread에 해당하는 page_table이 존재한다는 뜻!

//supplement page_table의 entry
struct page_entry
{
    void* vaddr; //user address의 가상주소
    int flag; //추가 정보를 담고 있는 flag

    struct hash_elem elem;
};

/* flag 관련 */
//page flag
#define p_present 0x1 // frame에 올라옴(활성화) frame에 안 올라와있음.(비활성화)
#define p_write 0x2; // read/write (활성화) read (비활성화)
#define p_pinning 0x4; //evict 불가능(활성화) evict 가능(비활성화)
#define p_access 0x10; //access_bit와 같음
#define p_dirty 0x20; //dirty_bit와 같음

//원하는 flag를 올리고 싶으면 p_present|p_write 이런 식으로 세팅
//활성화 안 하고 싶으면 0으로

/* page table 관련 */
//page_table을 초기화해줌.
bool init_page_table(struct hash* pt);
//page_table을 해제함.
bool destory_page_table(struct hash* pt);

/* page entry 관련 */
//page_entry를 현재 thread의 page_table에 넣는다.
bool set_page_entry(void* vaddr, int flag);
//hash_elem으로부터 page_entry 구조체를 얻어온다.
struct page_entry* elem_to_page_entry(const struct hash_elem *elem);
//현재 thread의 page_table에서 원하는 가상주소에 대한 page_entry를 가져온다.
struct page_entry* get_page_entry(void* vaddr);
//현재 thread의 page_table에서 page_entry를 제거한다. (하나 뻄. pop = true일 때)
//현재 thread의 page_table에서 page_entry 정보를 free한다 (destory용. pop = false)일 때.
bool delete_page_entry(struct page_entry* pte, bool pop);





/* stack grow 관련 */
//현재 thread의 stack을 최대 8MB까지 늘릴 수 있게 한다.
bool stack_grow(void* vaddr);

#endif
