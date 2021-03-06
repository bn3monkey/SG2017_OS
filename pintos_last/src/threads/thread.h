#ifndef THREADS_THREAD_H
#define THREADS_THREAD_H

#include <debug.h>
#include <list.h>
#include <hash.h>
#include <stdint.h>
#include "threads/synch.h"
//#include "filesys/filesys.h"

//#define DEBUGTHREAD
#ifndef USERPROG
extern bool thread_prior_aging;
#endif


/* States in a thread's life cycle. */
enum thread_status
  {
    THREAD_RUNNING,     /* Running thread. */
    THREAD_READY,       /* Not running but ready to run. */
    THREAD_BLOCKED,     /* Waiting for an event to trigger. */
    THREAD_DYING        /* About to be destroyed. */
  };

/* Thread identifier type.
   You can redefine this to whatever type you like. */
typedef int tid_t;
#define TID_ERROR ((tid_t) -1)          /* Error value for tid_t. */

/* Thread priorities. */
#define PRI_MIN 0                       /* Lowest priority. */
#define PRI_DEFAULT 31                  /* Default priority. */
#define PRI_MAX 63                      /* Highest priority. */

/* Start Added Context of Project 2 */
#define NICE_MAX 20
#define NICE_MIN -20
/* End Added Context of Project 2 */

#define MAX_OPENFILE 128

/* A kernel thread or user process.

   Each thread structure is stored in its own 4 kB page.  The
   thread structure itself sits at the very bottom of the page
   (at offset 0).  The rest of the page is reserved for the
   thread's kernel stack, which grows downward from the top of
   the page (at offset 4 kB).  Here's an illustration:

        4 kB +---------------------------------+
             |          kernel stack           |
             |                |                |
             |                |                |
             |                V                |
             |         grows downward          |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             +---------------------------------+
             |              magic              |
             |                :                |
             |                :                |
             |               name              |
             |              status             |
        0 kB +---------------------------------+

   The upshot of this is twofold:

      1. First, `struct thread' must not be allowed to grow too
         big.  If it does, then there will not be enough room for
         the kernel stack.  Our base `struct thread' is only a
         few bytes in size.  It probably should stay well under 1
         kB.

      2. Second, kernel stacks must not be allowed to grow too
         large.  If a stack overflows, it will corrupt the thread
         state.  Thus, kernel functions should not allocate large
         structures or arrays as non-static local variables.  Use
         dynamic allocation with malloc() or palloc_get_page()
         instead.

   The first symptom of either of these problems will probably be
   an assertion failure in thread_current(), which checks that
   the `magic' member of the running thread's `struct thread' is
   set to THREAD_MAGIC.  Stack overflow will normally change this
   value, triggering the assertion. */

/* The `elem' member has a dual purpose.  It can be an element in
   the run queue (thread.c), or it can be an element in a
   semaphore wait list (synch.c).  It can be used these two ways
   only because they are mutually exclusive: only a thread in the
   ready state is on the run queue, whereas only a thread in the
   blocked state is on a semaphore wait list. */

  struct thread
  {
    /* Owned by thread.c. */
    tid_t tid;                          /* Thread identifier. */
    enum thread_status status;          /* Thread state. */
    char name[16];                      /* Name (for debugging purposes). */
    uint8_t *stack;                     /* Saved stack pointer. */
    int priority;                       /* Priority. */
    struct list_elem allelem;           /* List element for all threads list. */

    /* Shared between thread.c and synch.c. */
    struct list_elem elem;              /* List element. */

    /* Start Added Context of Project 1 */
    struct list_elem elem_child; /* List element for child of this thread */
    struct list list_child; /* the list of child thread */
    #ifdef DEBUGTHREAD
    struct list_elem* elem_parent; /* List element for parent of this thread */
    #endif

    struct semaphore dead_sema; /* lock before the parent's dying! */
    struct semaphore wait_sema; /* lock for using the parent's wait! */

    //bool has_been_waiting; // checked when this thread has been waiting
    bool load_fail;
    int exit_status; // the status of exit

    struct semaphore load_sema; /* for maintaining process_execute while load ends */
    struct semaphore execute_sema;  /* for maintaining load while process_execute gets the exit_status */

    /* End Added Context of Project 1 */

    /* Start Added Context of Project 1-2 */
    struct file* fd_table[MAX_OPENFILE]; /* FILE DESCRIPTOR TABLE */
    int table_top; // table is managed like stack, so it has a top. 
    struct file* processfile; /* the process file. it cannot be written while process ends. */
    
    /* Start Added Context of Project 2 */
    int64_t end_sleep;
    int nice;
    int recent_cpu; //fixed
    /* End Added Context of Project 2 */

#ifdef USERPROG
    /* Owned by userprog/process.c. */
    uint32_t *pagedir;                  /* Page directory. */
#endif

    /* Start Added Context Of Project 3 */
    struct hash pt;
    uint32_t *esp;
    /* End Added Context Of Project 3 */

    /* Owned by thread.c. */
    unsigned magic;                     /* Detects stack overflow. */
  };

/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
extern bool thread_mlfqs;

void thread_init (void);
void thread_start (void);

void thread_tick (void);
void thread_print_stats (void);

typedef void thread_func (void *aux);
tid_t thread_create (const char *name, int priority, thread_func *, void *);

void thread_block (void);
void thread_unblock (struct thread *);

struct thread *thread_current (void);
tid_t thread_tid (void);
const char *thread_name (void);

void thread_exit (void) NO_RETURN;
void thread_yield (void);



/* Performs some operation on thread t, given auxiliary data AUX. */
typedef void thread_action_func (struct thread *t, void *aux);
void thread_foreach (thread_action_func *, void *);

int thread_get_priority (void);
void thread_set_priority (int);

int thread_get_nice (void);
void thread_set_nice (int);
int thread_get_recent_cpu (void);
int thread_get_load_avg (void);


/* Start Added Context of Project 1 */
#ifdef DEBUGTHREAD
struct thread* getParent(struct thread* t);
#endif
struct thread* getChild_byElem(struct list_elem* elem);
struct thread* getChild_byList(struct thread* t, tid_t tid);
struct thread* getChild_byList_nonremove(struct thread* t, tid_t tid);

void alertThread(const char* debugmsg, struct thread* t);
/* End Added Context of Project 1 */

/* Start Added COntext of Project 2 */
bool less_thread_endsleep(const struct list_elem* a, const struct list_elem* b, void* aux);
bool greater_thread_priority(const struct list_elem* a, const struct list_elem* b, void* aux);

void priority_update(struct thread* t);
//yield해야 되면 true다. 
bool priority_allupdate(void);

void recent_cpu_increment(void);
void recent_cpu_update(struct thread *t);
void recent_cpu_allupdate(void);

void load_avg_update(void);


struct thread* getThread_byElem(struct list_elem * elem);

void thread_sleep(int64_t end);

void thread_awake(void);

struct thread* next_thread(void);

void printReadythread(void);
/* End Added COntext of Project 2 */

#endif /* threads/thread.h */
