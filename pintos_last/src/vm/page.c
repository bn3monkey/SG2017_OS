#include <string.h>
#include <stdbool.h>
#include "vm/page.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/malloc.h"


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
bool set_page_entry(void* vaddr, int flag)
{
    struct page_entry* entry = (struct page_entry *)malloc(sizeof(struct page_entry));
    if(!entry) return false;

    entry->vaddr = pg_round_down(vaddr);
    entry->flag = flag;

    return true;
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
    if(elem == NULL) return NULL;
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

/* stack grow 관련 */
//현재 thread의 stack을 최대 8MB까지 늘릴 수 있게 한다.
bool stack_grow(void* vaddr)
{
    vaddr = NULL;
    return true;
}
