#include <string.h>
#include <stdbool.h>
#include "vm/page.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/malloc.h"
#include "userprog/pagedir.h"
#include "userprog/syscall.h"
#include "userprog/process.h"
#include "filesys/file.h"
#include "threads/interrupt.h"

#include <stdio.h>

void file_lock_acquire(void);
void file_lock_release(void);

/* flag 관련 */
int get_fetch(struct page_entry* entry)
{
    return entry->flag & 0xf;
}
int set_fetch(struct page_entry* entry, int new)
{
    return entry->flag = ((entry->flag & 0xf0) | new);
}
int get_pin(struct page_entry* entry)
{
    return entry->flag & 0x10;
}
int set_pin(struct page_entry* entry, int pinning)
{
    return entry->flag = ((entry->flag & 0x2f) | pinning);
}
int get_writebit(struct page_entry* entry)
{
    return entry->flag >> 5;
}
int get_write(struct page_entry* entry)
{
    return entry->flag & 0x20;
}
int set_write(struct page_entry* entry, int writable)
{
   return entry->flag = ((entry->flag & 0x1f) | writable);
}

/* page table 관련 */

static unsigned int page_hash_func (const struct hash_elem *elem, void *aux UNUSED)
{
  return hash_int((int)(elem_to_page_entry(elem)->vaddr));
}
static bool page_less_func (const struct hash_elem *a, const struct hash_elem *b, void *aux UNUSED)
{
  return (elem_to_page_entry(a)->vaddr < elem_to_page_entry(b)->vaddr);
}
static void page_action_func (struct hash_elem *elem, void *aux UNUSED)
{
    struct page_entry* entry = elem_to_page_entry((const struct hash_elem *)elem);
    delete_page_entry(entry, false);
}

//page_table을 초기화해줌.
bool init_page_table(struct hash* pt)
{
    if(hash_init (pt, page_hash_func, page_less_func, NULL))
        return true;
    return false;
}
//page_table을 해제함.
bool destory_page_table(struct hash* pt)
{
    hash_destroy(pt, page_action_func);
    return true;
}

/* page entry 관련 */
//page_entry를 현재 thread의 page_table에 넣는다.
struct page_entry* set_page_entry(void* vaddr, int flag)
{
    struct page_entry* entry = (struct page_entry *)malloc(sizeof(struct page_entry));
    if(!entry) return NULL;

    entry->vaddr = pg_round_down(vaddr);
    entry->flag = flag;

    //중복된 게 안에 있으면, vaddr은 하나의 page_table에 한 개 밖에 있을 수 없기 때문에 잘 못 된 것이다.
    if(hash_insert(&thread_current()->pt, &entry->elem) != NULL )
    {
        free(entry);
        entry = NULL;
    }
    return entry;
}
//파일에서 불러오는 것들.
//file_system에 관한 page_entry를 page_table에 넣는다.
struct page_entry*  set_page_entry_from_file(void* vaddr, struct file* file, size_t offset, size_t read_bytes, bool writable)
{
    int read_or_write = writable ? p_write : p_read;
    struct page_entry* entry = set_page_entry(vaddr, (p_nfile | p_unpin | read_or_write));
    if(!entry) return NULL;
    entry->pf.file = file;
    entry->pf.offset = offset;
    entry->pf.read_bytes = read_bytes;
    return entry;
}
//파일에서 불러오지 않은 것들.
//swap_system에 관한 page_entry를 page_table에 넣는다. 
struct page_entry*  set_page_entry_from_swap(void* vaddr, size_t swap_index)
{
    struct page_entry* entry = set_page_entry(vaddr, (p_nswap | p_unpin | p_write));
     
    if(!entry) return NULL;
    entry->swap_index = swap_index;
    return entry;
}
//grow_stack 같이 kernel로 들어가는 것과 들어가지 않을 것도 고려.

//all_zero에 관한 page_entry를 page_table 넣는다. 
struct page_entry*  set_page_entry_from_allzero(void* vaddr)
{
    return vaddr = NULL;
}

//hash_elem으로부터 page_entry 구조체를 얻어온다.
struct page_entry* elem_to_page_entry(const struct hash_elem *elem)
{
    return hash_entry(elem, struct page_entry, elem);
}
//현재 thread의 page_table에서 원하는 가상주소에 대한 page_entry를 가져온다.
struct page_entry* get_page_entry(void* vaddr)
{
    //vaddr에 해당하는 가장 가까운 page를 찾는다.
    struct page_entry temp;
    temp.vaddr = pg_round_down(vaddr);

    struct hash_elem* elem = hash_find(&thread_current()->pt, &temp.elem);
    if(elem == NULL) 
        return NULL;
    return elem_to_page_entry(elem);
}
//현재 thread의 page_table에서 page_entry를 제거한다. (하나 뻄. pop = true일 때)
//현재 thread의 page_table에서 page_entry 정보를 free한다 (destory용. pop = false)일 때.
bool delete_page_entry(struct page_entry* pte, bool pop)
{
    if(pop)
        hash_delete(&(thread_current()->pt), &(pte->elem));
    free(pte);
    return true;
}

/* fetch 관련 */
// page_entry를 알맞은 frame에 연결시킨다.
bool fetch_page(struct page_entry* pte)
{
    //1. kernel process를 수행하는 도중에는, 
    //   page가 도중에 eviction되지 않도록 할 필요가 있다.
    //   따라서 pin flag로 해당 page를 잠근다.

    //2. kernel process일 때는
    // 주로 page_fault나 system call 수행 중일 때이다.
    // 끝날 때쯤 해제하면 된다.
    set_pin(pte, p_pin);

    //2. page_entry에 이미 저장된 fetch할 type대로 fetch하면 된다.
    switch(get_fetch(pte))
    {
        case p_nonfetched : return false;
        case p_file : return fetch_fromfile(pte);
        case p_swap : return fetch_fromswap(pte);
        //case p_allzero : return fetch_fromeallzero(pte);
    }
    return false;
}
// page_entry를 file로부터 frame에 fetch한다.
bool fetch_fromfile(struct page_entry* pte)
{
    size_t real_bytes;
    void* paddr;
    //1. 우선 현재 pte로부터 frame을 얻어온다.
    if(!link_page_entry(pte, &paddr))
        return false;

    //2. 처음 page 내부의 값을 전부 0을 세팅한다.
    memset(paddr, 0 , PGSIZE);     

    if(pte->pf.read_bytes > 0)
    {
        //3. paddr에 read_byte만큼 파일에서 값을 채워넣는다.
        file_lock_acquire();
        real_bytes = (size_t)file_read_at(pte->pf.file, paddr, pte->pf.read_bytes, pte->pf.offset);
        file_lock_release();
    
        //4. 읽어야하는 값이랑 실제랑 읽은 값이 맞지 않다면
        //5. 매핑한 frame을 해제해주고 false를 return한다.
        if(real_bytes != pte->pf.read_bytes)
        {
            unlink_page_entry(pte);
            return false;
        }
    }

    //5. flag로 file에 대해 fetch됐다고 표시한다.
    set_fetch(pte, p_file);
    return true;
}
// page_entry를 swap으로부터 frame에 fetch한다.
bool fetch_fromswap(struct page_entry* pte)
{
    void* paddr;
     //1. 우선 현재 pte로부터 frame을 얻어온다.
    if(!link_page_entry(pte, &paddr))
        return false;

    //2. swap_disk에서 가져온다.
    //swap_in

    //3. flag로 swap에 대해 fetch됐다고 표시한다.
    set_fetch(pte, p_swap);
    return true;
}

// page_entry를 allzero로부터 frame에 fetch한다.
bool fetch_fromallzero(struct page_entry* pte)
{
   memset (page_to_frame(pte), 0 ,PGSIZE);
   return true;
}

/* link 관련 */
//Page Entry에 frame을 얻어와 연결시킨다.
//제대로 수행되었으면 true를 리턴한다.
bool link_page_entry(struct page_entry* pte, void** paddr)
{
    //1. frame을 frame_table에 올린다.
    *paddr = set_frame_entry(pte);
    if(*paddr != NULL)
    {
        //2. frame을 page_directory에 올린다.
        if(install_page(pte->vaddr, *paddr, get_writebit(pte)))
            return true;
        else
            delete_frame_entry(*paddr);
    }
    //둘 중 하나라도 실패하면 가상주소와 물리 주소 연결이 안 된 것으로
    //간주하여 false를 리턴한다.
    return false;
}
//Page Entry에 있는 frame의 연결을 끊고, frame을 해제한다.
//제대로 수행되었으면, true를 리턴한다.
bool unlink_page_entry(struct page_entry* pte)
{
    bool success = false;
    //1. frame_entry에서 제거한다.
    if(delete_frame_entry(page_to_frame(pte)))
        success = true;
    //2. page_directory에서 제거한다.
    pagedir_clear_page(thread_current()->pagedir, pte->vaddr);
    return success;
}
//page Entry에서 frame을 가져온다. (frame_entry가 아니다.)
void* page_to_frame(struct page_entry* entry)
{
    if(entry == NULL)
        return NULL;
    return pagedir_get_page(thread_current()->pagedir, entry->vaddr);
}

/* stack grow 관련 */
//현재 thread의 stack을 최대 8MB까지 늘릴 수 있게 한다.
bool stack_grow(void* vaddr, void* esp, void** out_pte)
{
    //1. push에 어느 정도 오차가 있기 때문에 32 bit 안쪽에 가상주소가 있으면,
    //   스택으로 친다. 하지만 바깥에 있으면 예외처리한다. 
    if(vaddr < esp - 32)
    {
        //printf("vaddr : %x  esp :  %x\n",vaddr,esp);
       // printf("esp - stack = %x\n",esp - vaddr);
        return false;
    }
    //2. 현재 있는 가상주소가 최대 스택 사이즈를 벗어나도, 예외처리를 한다.
    if((size_t)pg_round_down(vaddr) < (size_t)PHYS_BASE - MAX_STACK)
    {
        //printf("virtual address very low\n");
        return false;
    }
    //3. 새로 할당할 스택 영역에 대해, PAGE_TABLE_ENTRY를 만든다.
    struct page_entry* pte;
    pte = set_page_entry_from_swap(pg_round_down(vaddr), 0);
    //page_table_entry가 안 만들어지면 false.
    if(pte==NULL)
        return false;
    //4. stack 영역은 file에서 받아오는 것이 아니므로, SWAP_DISK 영역과 교환한다.
    set_fetch(pte, p_swap);
    set_pin(pte, p_pin);

     //printf("pte success\n");
        

    //5. 임시로 frame을 담을 변수를 만든 뒤, pte와 새로 할당한 frame을 연결한다.
    //6. 연결할 수 없으면, false를 리턴한다.
    void* frame;
    if(!link_page_entry(pte, &frame)) //실패시 알아서 frame과 pte를 할당해제해준다.
        return false;

    //printf("link success\n");

    //7. 성공했으니 page_table을 바깥으로 내보내준다.
    if(out_pte != NULL)
        *out_pte = pte;

    return true;
}
