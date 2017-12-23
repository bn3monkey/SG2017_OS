#ifndef __VM_PAGE__
#define __VM_PAGE__

#include <hash.h>
#include "vm/frame.h"

#define MAX_STACK (1<<23)

//struct thread 내에 struct hash pt 구조체가 있다.
//thread마다 thread에 해당하는 page_table이 존재한다는 뜻!

//file로부터 fetch됐을 때 다음과 같은 자료 구조를 사용한다.
//flag가 0x03일 때 작용.
struct page_file
{
    struct file* file;
    size_t offset;
    size_t read_bytes;
    size_t zero_bytes;
};
//flag가 0x05일 때 작용.
//size_t swap_index가 쓰임

//supplement page_table의 entry
struct page_entry
{
    void* vaddr; //user address의 가상주소
    int flag; //추가 정보를 담고 있는 flag

    union
    {
        struct page_file pf; //파일에서 fetch됐을 때
        size_t swap_index; //swap에서 fetch 됐을 때
    };
    struct hash_elem elem;
};

/* flag 관련 */
//page flag
#define p_nonfetched 0x0 //frame에서 fetch되지 않음.
#define p_fetched 0x1 // frame이 이미 fetch되었을 때
#define p_nfile 0x2 // file에서 nonfetch됨.
#define p_file 0x3 // file에서 fetch됨.
#define p_nswap 0x4 // swap에서 nonfetch됨.
#define p_swap 0x5 // swap에서 fetch됨. 
#define p_nallzero 0x8 //nonozero에서 nonfetch됨
#define p_allzero 0x9 //모두 zero로 fetch됨.

#define p_unpin 0x00 //evict 불가능
#define p_pin 0x10 // evict 가능

#define p_write 0x20// 읽고 쓸 수 있음.
#define p_read 0x00// 읽기만 가능.

int get_fetch(struct page_entry* entry);
int set_fetch(struct page_entry* entry, int new);
int get_pin(struct page_entry* entry);
int set_pin(struct page_entry* entry, int pinning);
int get_writebit(struct page_entry* entry);
int get_write(struct page_entry* entry);
int set_write(struct page_entry* entry, int writable);

//원하는 flag를 올리고 싶으면 p_present|p_write 이런 식으로 세팅
//활성화 안 하고 싶으면 0으로

/* page table 관련 */
//page_table을 초기화해줌.
bool init_page_table(struct hash* pt);
//page_table을 해제함.
bool destory_page_table(struct hash* pt);

/* page entry 관련 */
//page_entry를 현재 thread의 page_table에 넣는다.
struct page_entry*  set_page_entry(void* vaddr, int flag);
//file_system에 관한 page_entry를 page_table에 넣는다.
struct page_entry*  set_page_entry_from_file(void* vaddr, struct file* file, size_t offset, size_t read_bytes, size_t zero_bytes);
//swap_system에 관한 page_entry를 page_table에 넣는다. 
struct page_entry*  set_page_entry_from_swap(void* vaddr, size_t swap_index);
//all_zero에 관한 page_entry를 page_table 넣는다. 
struct page_entry*  set_page_entry_from_allzero(void* vaddr);

//hash_elem으로부터 page_entry 구조체를 얻어온다.
struct page_entry* elem_to_page_entry(const struct hash_elem *elem);
//현재 thread의 page_table에서 원하는 가상주소에 대한 page_entry를 가져온다.
struct page_entry* get_page_entry(void* vaddr);
//현재 thread의 page_table에서 page_entry를 제거한다. (하나 뻄. pop = true일 때)
//현재 thread의 page_table에서 page_entry 정보를 free한다 (destory용. pop = false)일 때.
bool delete_page_entry(struct page_entry* pte, bool pop);

/* fetch 관련 */
// page_entry를 알맞은 frame에 연결시킨다.
bool fetch_page(struct page_entry* pte);
// page_entry를 file로부터 frame에 fetch한다.
bool fetch_fromfile(struct page_entry* pte);
// page_entry를 swap으로부터 frame에 fetch한다.
bool fetch_fromswap(struct page_entry* pte);
// page_entry를 allzero로부터 frame에 fetch한다.
bool fetch_fromallzero(struct page_entry* pte);

/* link 관련 */
//Page Entry에 frame을 얻어와 연결시킨다.
bool link_page_entry(struct page_entry* pte, void** frame);
//Page Entry에 있는 frame의 연결을 끊고, frame을 해제한다.
bool unlink_page_entry(struct page_entry* pte);
//page Entry에서 frame을 가져온다. (frame_entry가 아니다.)
void* page_to_frame(struct page_entry* entry); 

/* stack grow 관련 */
//현재 thread의 stack을 최대 8MB까지 늘릴 수 있게 한다.
bool stack_grow(void* vaddr);

#endif
