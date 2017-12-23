#include "threads/thread.h"
#include <debug.h>
#include <stddef.h>
#include <random.h>
#include <stdio.h>
#include <string.h>
#include "threads/flags.h"
#include "threads/interrupt.h"
#include "threads/intr-stubs.h"
#include "threads/palloc.h"
#include "threads/malloc.h"
#include "threads/switch.h"
#include "threads/synch.h"
#include "threads/vaddr.h"

#ifdef USERPROG

#include "userprog/process.h"
#include "vm/frame.h"
#include "vm/page.h"

#endif

#include "filesys/file.h"
#include "devices/timer.h"
#include "threads/fixed_point.h"



/* Random value for struct thread's `magic' member.
   Used to detect stack overflow.  See the big comment at the top
   of thread.h for details. */
#define THREAD_MAGIC 0xcd6abf4b

/* List of processes in THREAD_READY state, that is, processes
   that are ready to run but not actually running. */
static struct list ready_list[PRI_MAX+1];

/* Start Added Context of Project 2 */
//List which is sleeping
static struct list sleep_list;
static int load_avg; //fixed_point 자료형
#define NICE_DEFAULT 0
#define RECENT_CPU_DEFAULT 0
#define LOAD_AVG_DEFAULT 0
/* End Added Context of Project 2 */

/* List of all processes.  Processes are added to this list
   when they are first scheduled and removed when they exit. */
static struct list all_list;

/* Idle thread. */
static struct thread *idle_thread;

/* Initial thread, the thread running init.c:main(). */
static struct thread *initial_thread;

/* Lock used by allocate_tid(). */
static struct lock tid_lock;


/* Stack frame for kernel_thread(). */
struct kernel_thread_frame 
  {
    void *eip;                  /* Return address. */
    thread_func *function;      /* Function to call. */
    void *aux;                  /* Auxiliary data for function. */
  };

/* Statistics. */
static long long idle_ticks;    /* # of timer ticks spent idle. */
static long long kernel_ticks;  /* # of timer ticks in kernel threads. */
static long long user_ticks;    /* # of timer ticks in user programs. */

/* Scheduling. */
#define TIME_SLICE 4            /* # of timer ticks to give each thread. */
static unsigned thread_ticks;   /* # of timer ticks since last yield. */

#ifndef USERPROG
bool thread_prior_aging;
#endif

/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
bool thread_mlfqs;

static void kernel_thread (thread_func *, void *aux);

static void idle (void *aux UNUSED);
static struct thread* next_thread_to_run (void);
static struct thread *running_thread (void);
static void init_thread (struct thread *, const char *name, int priority);
static bool is_thread (struct thread *) UNUSED;
static void *alloc_frame (struct thread *, size_t size);
static void schedule (void);
void thread_schedule_tail (struct thread *prev);
static tid_t allocate_tid (void);


static inline void alertAllThread(const char* name)
{
  printf("%s's all thread\n",name);

  struct list_elem* e;
  for (e = list_begin (&all_list); e != list_end (&all_list);
     e = list_next (e))
   {
     struct thread *t = list_entry (e, struct thread, allelem);
     printf("tid : %d , name : %s status : ", t->tid, t->name);
     switch(t->status)
     {
       case 0 : printf("THREAD_RUNNING\n"); break;
       case 1 : printf("THREAD_READY\n"); break;
       case 2 : printf("THREAD_BLOCKED\n"); break;
       case 3 : printf("THREAD_DYING\n"); break;
     }
    }  
}

#ifdef DEBUGTHEAD
static inline void alertChildThread(struct thread* parent)
{
  struct thread *t;
   
  printf("%s's child thread\n",parent->name);

  struct list_elem* e;
  struct list* childlist = &(parent->list_child);
  for (e = list_begin (childlist); e != list_end (childlist);
     e = list_next (e))
   {
     t = list_entry (e, struct thread, elem_child);
     printf("tid : %d , name : %s status : ", t->tid, t->name);
     switch(t->status)
     {
       case 0 : printf("THREAD_RUNNING\n"); break;
       case 1 : printf("THREAD_READY\n"); break;
       case 2 : printf("THREAD_BLOCKED\n"); break;
       case 3 : printf("THREAD_DYING\n"); break;
     }
    }  
}


void alertThread(const char* debugmsg, struct thread* t)
{
  printf("Check in %s\n",debugmsg);
  printf("tid : %d , name : %s status : ", t->tid, t->name);
  switch(t->status)
  {
    case 0 : printf("THREAD_RUNNING\n"); break;
    case 1 : printf("THREAD_READY\n"); break;
    case 2 : printf("THREAD_BLOCKED\n"); break;
    case 3 : printf("THREAD_DYING\n"); break;
  }
  alertChildThread(t);
  alertAllThread(t->name);

}


static inline void alertSiblingThread(struct thread* sibling)
{
  struct thread *t;
   
  printf("%s(%d)'s sibling thread\n",sibling->name,sibling->tid);

  struct list_elem* e;
  for (e = &(sibling->elem_child); e != NULL;
     e = e->prev)
   {
     t = list_entry (e, struct thread, elem_child);
     if(0<=t->status && t->status<=3)
     {
      printf("tid : %d , name : %s status : ", t->tid, t->name);
       switch(t->status)
      {
       
        case 0 : printf("THREAD_RUNNING\n"); break;
        case 1 : printf("THREAD_READY\n"); break;
        case 2 : printf("THREAD_BLOCKED\n"); break;
        case 3 : printf("THREAD_DYING\n"); break;      
      }
     } 
    }
  for (e = sibling->elem_child.next; e != NULL;
    e = e->next)
   {
     t = list_entry (e, struct thread, elem_child);
     if (0 <= t->status && t->status <= 3)
     {
       printf("tid : %d , name : %s status : ", t->tid, t->name);
       switch (t->status)
       {
       case 0:
         printf("THREAD_RUNNING\n");
         break;
       case 1:
         printf("THREAD_READY\n");
         break;
       case 2:
         printf("THREAD_BLOCKED\n");
         break;
       case 3:
         printf("THREAD_DYING\n");
         break;
       }
     }
   }
}
#endif

/* Initializes the threading system by transforming the code
   that's currently running into a thread.  This can't work in
   general and it is possible in this case only because loader.S
   was careful to put the bottom of the stack at a page boundary.

   Also initializes the run queue and the tid lock.

   After calling this function, be sure to initialize the page
   allocator before trying to create any threads with
   thread_create().

   It is not safe to call thread_current() until this function
   finishes. */

void
thread_init (void) 
{
  ASSERT (intr_get_level () == INTR_OFF);
  int i;
  lock_init (&tid_lock);
  for(i=0;i<=PRI_MAX;i++)
    list_init (ready_list+i);
  list_init (&all_list);

  /* Start Added Context of Project 2 */
  list_init(&sleep_list);
  /* End Added Context of Project 2 */

  /* Start Added Context of Project 3 */
  //page_table을 process.c의 load에서 초기화해준다.
  //frame_table은 thread.c의 init_thread에서 초기화해준다.
  init_frame_table();
  /* ENd Added Context of Project 3 */

  /* Set up a thread structure for the running thread. */
  initial_thread = running_thread ();
  init_thread (initial_thread, "main", PRI_DEFAULT);
  initial_thread->status = THREAD_RUNNING;
  initial_thread->tid = allocate_tid ();
}

/* Starts preemptive thread scheduling by enabling interrupts.
   Also creates the idle thread. */
void
thread_start (void) 
{
  /* Create the idle thread. */
  struct semaphore start_idle;
  sema_init (&start_idle, 0);
  thread_create ("idle", PRI_MIN, idle, &start_idle);

  load_avg  = LOAD_AVG_DEFAULT;

  /* Start preemptive thread scheduling. */
  intr_enable ();

  /* Wait for the idle thread to initialize idle_thread. */
  sema_down (&start_idle);
}

/* Called by the timer interrupt handler at each timer tick.
   Thus, this function runs in an external interrupt context. */
void
thread_tick (void) 
{
  struct thread *t = thread_current ();
  bool yieldflag = true;

  /* Update statistics. */
  if (t == idle_thread)
    idle_ticks++;
#ifdef USERPROG
  else if (t->pagedir != NULL)
    user_ticks++;
#endif
  else
    kernel_ticks++;

  
  recent_cpu_increment();
  if((timer_ticks()+1)%TIMER_FREQ == 0)
  {
    load_avg_update();
    recent_cpu_allupdate();
  }
  #ifndef USERPROG
  if(thread_prior_aging || thread_mlfqs)
  {
    if((thread_ticks+1)%4==0)
    {
        yieldflag = priority_allupdate(); 
    }
  }
  #endif
 //printReadythread();
  //printf("thread_tick : %d timer_tick : %d\n",thread_ticks+1,(int)(timer_ticks()+1));

  /* Enforce preemption. */
  if (++thread_ticks >= TIME_SLICE && yieldflag)
    intr_yield_on_return ();

}

/* Prints thread statistics. */
void
thread_print_stats (void) 
{
  printf ("Thread: %lld idle ticks, %lld kernel ticks, %lld user ticks\n",
          idle_ticks, kernel_ticks, user_ticks);
}

/* Creates a new kernel thread named NAME with the given initial
   PRIORITY, which executes FUNCTION passing AUX as the argument,
   and adds it to the ready queue.  Returns the thread identifier
   for the new thread, or TID_ERROR if creation fails.

   If thread_start() has been called, then the new thread may be
   scheduled before thread_create() returns.  It could even exit
   before thread_create() returns.  Contrariwise, the original
   thread may run for any amount of time before the new thread is
   scheduled.  Use a semaphore or some other form of
   synchronization if you need to ensure ordering.

   The code provided sets the new thread's `priority' member to
   PRIORITY, but no actual priority scheduling is implemented.
   Priority scheduling is the goal of Problem 1-3. */
tid_t
thread_create (const char *name, int priority,
               thread_func *function, void *aux) 
{
  struct thread *t;
  struct kernel_thread_frame *kf;
  struct switch_entry_frame *ef;
  struct switch_threads_frame *sf;
  tid_t tid;
  enum intr_level old_level;

  ASSERT (function != NULL);

  /* Allocate thread. */
  t = palloc_get_page (PAL_ZERO);
  if (t == NULL)
  {
    return TID_ERROR;
  }

  /* Initialize thread. */
  init_thread (t, name, priority);
  tid = t->tid = allocate_tid ();

  /* Start Added Context of Project 1 */
  //Make the thread the child of current thread.
  struct thread* current = thread_current();
  
  #ifdef DEBUGTHREAD 
  alertThread("thread_create : parent",current);
  alertThread("thread_create : child", t);
  #endif
  
  /* End Added Context of Project 1 */

  /* Start Added Context of Project 2 */
  ASSERT(NICE_MIN <= current->nice && current->nice <= NICE_MAX);
  t->nice = current->nice;
  t->recent_cpu = current->recent_cpu;

  //t->priority = current->priority;
  /* End Added Context of Project 2 */

  //link parent and child
  #ifdef DEBUGTHREAD
  t->elem_parent = &(current->elem_child); 
  #endif
  list_push_back(&(current->list_child), &(t->elem_child));
  

  /* Prepare thread for first run by initializing its stack.
     Do this atomically so intermediate values for the 'stack' 
     member cannot be observed. */
  old_level = intr_disable ();

  /* Stack frame for kernel_thread(). */
  kf = alloc_frame (t, sizeof *kf);
  kf->eip = NULL;
  kf->function = function;
  kf->aux = aux;

  /* Stack frame for switch_entry(). */
  ef = alloc_frame (t, sizeof *ef);
  ef->eip = (void (*) (void)) kernel_thread;

  /* Stack frame for switch_threads(). */
  sf = alloc_frame (t, sizeof *sf);
  sf->eip = switch_entry;
  sf->ebp = 0;

  intr_set_level (old_level);

  /* Add to run queue. */
  thread_unblock (t);


  if(intr_context()==false && t->priority > thread_current()->priority)
  {
    //ASSERT2(t,thread_current());
    thread_yield ();
  }


  return tid;
}

/* Puts the current thread to sleep.  It will not be scheduled
   again until awoken by thread_unblock().

   This function must be called with interrupts turned off.  It
   is usually a better idea to use one of the synchronization
   primitives in synch.h. */
void
thread_block (void) 
{
  ASSERT (!intr_context ());
  ASSERT (intr_get_level () == INTR_OFF);

  thread_current ()->status = THREAD_BLOCKED;
  schedule ();
}

/* Transitions a blocked thread T to the ready-to-run state.
   This is an error if T is not blocked.  (Use thread_yield() to
   make the running thread ready.)

   This function does not preempt the running thread.  This can
   be important: if the caller had disabled interrupts itself,
   it may expect that it can atomically unblock a thread and
   update other data. */
void
thread_unblock (struct thread *t) 
{
  enum intr_level old_level;

  ASSERT (is_thread (t));

  old_level = intr_disable ();
  ASSERT (t->status == THREAD_BLOCKED);
  /* Start Added Context of Project 2 */
  ASSERT(PRI_MIN <= t->priority && t->priority <= PRI_MAX);
  list_push_back (ready_list+(t->priority), &t->elem);
  t->status = THREAD_READY;

  /* End Added Context of Project 2 */
  intr_set_level (old_level);

}

/* Returns the name of the running thread. */
const char *    
thread_name (void) 
{
  return thread_current ()->name;
}

/* Returns the running thread.
   This is running_thread() plus a couple of sanity checks.
   See the big comment at the top of thread.h for details. */
struct thread *
thread_current (void) 
{
  struct thread *t = running_thread ();
  
  /* Make sure T is really a thread.
     If either of these assertions fire, then your thread may
     have overflowed its stack.  Each thread has less than 4 kB
     of stack, so a few big automatic arrays or moderate
     recursion can cause stack overflow. */



  ASSERT (is_thread (t));
  
  #ifdef DEBUGTHREAD
  if(t->status != THREAD_RUNNING)
    alertThread("there is no running thread",t);
  #endif

  ASSERT (t->status == THREAD_RUNNING);
  
  return t;
}

/* Returns the running thread's tid. */
tid_t
thread_tid (void) 
{
  return thread_current ()->tid;
}

/* Deschedules the current thread and destroys it.  Never
   returns to the caller. */
void
thread_exit (void) 
{
  ASSERT (!intr_context ());

#ifdef USERPROG
  process_exit ();
#endif

  /* Remove thread from all threads list, set our status to dying,
     and schedule another process.  That process will destroy us
     when it calls thread_schedule_tail(). */
  struct thread* current = thread_current();
  #ifdef DEBUGTHREAD
  alertThread("thread_exit", current);
  #endif

  #ifdef DEBUGTHREAD 
   alertThread("thread_exit when current", current);
  #endif

   /* Start Added Context of Project 1 */


   // Free deallocate all of the content that is added at Project 1
   
   // Disconnect the connection of parent and child.
   // 호적에서 파버린다.

 if(current->load_fail == true)
 {
    list_remove (&current->elem_child);
 }

  struct list* childlist = &(current->list_child);
  struct list_elem* childelem;
  struct thread* child;

  //alertSiblingThread(current);
  
  for (childelem = list_begin (childlist); childelem != list_end (childlist);)
  {
    child = list_entry(childelem, struct thread, elem_child);

    childelem = list_remove(childelem);
    sema_up(&(child->dead_sema));  

  }

  ASSERT(list_empty(childlist));

  sema_up(&(current->wait_sema));

  sema_down(&(current->dead_sema));

  /* End Added Context of Project 1 */

  //alertAllThread(current->name);
  list_remove (&current->allelem);

  //alertAllThread(current->name);

  current->status = THREAD_DYING;
  intr_disable ();

  schedule ();
  NOT_REACHED ();
}

/* Yields the CPU.  The current thread is not put to sleep and
   may be scheduled again immediately at the scheduler's whim. */
void
thread_yield (void) 
{
  struct thread *cur = thread_current ();
  enum intr_level old_level;
  
  ASSERT (!intr_context ());

  old_level = intr_disable ();
  if (cur != idle_thread)
  {
    ASSERT(PRI_MIN <= cur->priority && cur->priority <= PRI_MAX);
    list_push_back (ready_list+(cur->priority), &cur->elem);

    /*
    if(strcmp(getThread_byElem(list_front(ready_list+cur->priority))->name
      ,"main")!=0)
      {
    ASSERT2(
      getThread_byElem(list_front(ready_list+cur->priority)),
      getThread_byElem(list_back(ready_list+cur->priority))
    );
      }
    */
  }
  cur->status = THREAD_READY;
  schedule ();
  intr_set_level (old_level);
}

/* Invoke function 'func' on all threads, passing along 'aux'.
   This function must be called with interrupts off. */
void
thread_foreach (thread_action_func *func, void *aux)
{
  struct list_elem *e;

  ASSERT (intr_get_level () == INTR_OFF);

  for (e = list_begin (&all_list); e != list_end (&all_list);
       e = list_next (e))
    {
      struct thread *t = list_entry (e, struct thread, allelem);
      func (t, aux);
    }
}

/* Sets the current thread's priority to NEW_PRIORITY. */
void
thread_set_priority (int new_priority) 
{
  struct thread* current = thread_current();
  if(thread_mlfqs == true)
    return;

  if(current==NULL || current==idle_thread)
    return;
  current->priority = new_priority;
  ASSERT(PRI_MIN <= current->priority && current->priority <= PRI_MAX);
  if(intr_context()==false)
    if(current->priority < next_thread()->priority)
    {
      thread_yield ();
    }

}

/* Returns the current thread's priority. */
int
thread_get_priority (void) 
{
  return thread_current ()->priority;
}

/* Sets the current thread's nice value to NICE. */
void
thread_set_nice (int nice UNUSED) 
{
  struct thread* current = thread_current();
  /* Not yet implemented. */
   ASSERT(NICE_MIN <= current->nice && current->nice <= NICE_MAX);
   current->nice = nice;
  priority_update(current);
  if(current->priority < next_thread()->priority)
  {
    if(intr_context() == false)
        thread_yield();
  }
}

/* Returns the current thread's nice value. */
int
thread_get_nice (void) 
{
  struct thread* current = thread_current();
  /* Not yet implemented. */
  return current->nice;
}

/* Returns 100 times the system load average. */
int
thread_get_load_avg (void) 
{
  /* Not yet implemented. */
    int temp;
    temp = toint_r(
          multcompound(
            load_avg,
            100
            )
          );
    //printReadythread();
    return temp;
}

/* Returns 100 times the current thread's recent_cpu value. */
int
thread_get_recent_cpu (void) 
{
  /* Not yet implemented. */
  int temp;
  temp =  toint_r(
          multcompound(
            thread_current()->recent_cpu,
            100
            )
          );

  return temp;
}

/* Idle thread.  Executes when no other thread is ready to run.

   The idle thread is initially put on the ready list by
   thread_start().  It will be scheduled once initially, at which
   point it initializes idle_thread, "up"s the semaphore passed
   to it to enable thread_start() to continue, and immediately
   blocks.  After that, the idle thread never appears in the
   ready list.  It is returned by next_thread_to_run() as a
   special case when the ready list is empty. */
static void
idle (void *idle_started_ UNUSED) 
{
  struct semaphore *idle_started = idle_started_;
  idle_thread = thread_current ();
  sema_up (idle_started);

  for (;;) 
    {
      /* Let someone else run. */
      intr_disable ();
      thread_block ();

      /* Re-enable interrupts and wait for the next one.

         The `sti' instruction disables interrupts until the
         completion of the next instruction, so these two
         instructions are executed atomically.  This atomicity is
         important; otherwise, an interrupt could be handled
         between re-enabling interrupts and waiting for the next
         one to occur, wasting as much as one clock tick worth of
         time.

         See [IA32-v2a] "HLT", [IA32-v2b] "STI", and [IA32-v3a]
         7.11.1 "HLT Instruction". */
      asm volatile ("sti; hlt" : : : "memory");
    }
}

/* Function used as the basis for a kernel thread. */
static void
kernel_thread (thread_func *function, void *aux) 
{
  ASSERT (function != NULL);

  intr_enable ();       /* The scheduler runs with interrupts off. */
  function (aux);       /* Execute the thread function. */
  thread_exit ();       /* If function() returns, kill the thread. */
}

/* Returns the running thread. */
struct thread *
running_thread (void) 
{
  uint32_t *esp;

  /* Copy the CPU's stack pointer into `esp', and then round that
     down to the start of a page.  Because `struct thread' is
     always at the beginning of a page and the stack pointer is
     somewhere in the middle, this locates the curent thread. */
  asm ("mov %%esp, %0" : "=g" (esp));
  return pg_round_down (esp);
}

/* Returns true if T appears to point to a valid thread. */
static bool
is_thread (struct thread *t)
{
  return t != NULL && t->magic == THREAD_MAGIC;
}

/* Does basic initialization of T as a blocked thread named
   NAME. */
static void
init_thread (struct thread *t, const char *name, int priority)
{
  ASSERT (t != NULL);
  ASSERT (PRI_MIN <= priority && priority <= PRI_MAX);
  ASSERT (name != NULL);

  memset (t, 0, sizeof *t);
  t->status = THREAD_BLOCKED;
  strlcpy (t->name, name, sizeof t->name);
  t->stack = (uint8_t *) t + PGSIZE;
  t->priority = priority;
  t->nice = NICE_DEFAULT;
  t->recent_cpu = RECENT_CPU_DEFAULT;
  t->magic = THREAD_MAGIC;
  t->exit_status = 0;
  list_push_back (&all_list, &t->allelem);  

  /* Start Added Context of Project 1 */
  //init the variable of struct thread that is added in Project 1

  //initiate list_child of thread that will be made
  list_init (&(t->list_child));
  
  // Fisrt, the pointer of parent makes null
  #ifdef DEBUGTHREAD
  t->elem_parent = NULL;
  #endif
  // acquired when parent is wating
  // and release when parent ends wating (child die) 
  sema_init_wait(&(t->wait_sema));
  sema_init_wait(&(t->dead_sema));



  //for waiting end of load in process_execute.
  sema_init_wait((&(t->load_sema)));
  sema_init_wait(&(t->execute_sema));
  
  //First, has_been_waiting is false
  //t->has_been_waiting = false; 
  t->load_fail = false;
  /* End Added Context of Project 1 */

  /* Start Added Context of Project 1-2 */
  //Share the file lock and this file lock uses for making critical section
  t->table_top = 0;
  int i;
  for(i=0;i<MAX_OPENFILE;i++)
    t->fd_table[i] = NULL;
  t->processfile = NULL;
  /* End Added Context of Project 1-2 */


}

/* Allocates a SIZE-byte frame at the top of thread T's stack and
   returns a pointer to the frame's base. */
static void *
alloc_frame (struct thread *t, size_t size) 
{
  /* Stack data is always allocated in word-size units. */
  ASSERT (is_thread (t));
  ASSERT (size % sizeof (uint32_t) == 0);

  t->stack -= size;
  return t->stack;
}

/* Chooses and returns the next thread to be scheduled.  Should
   return a thread from the run queue, unless the run queue is
   empty.  (If the running thread can continue running, then it
   will be in the run queue.)  If the run queue is empty, return
   idle_thread. */
struct thread *
next_thread_to_run (void) 
{
  int i;
  for(i=PRI_MAX;i>=PRI_MIN;i--)
  {
    if(!list_empty(ready_list+i))
      return list_entry (list_pop_front (ready_list+i), struct thread, elem);
  }
  return idle_thread;
}

/* Completes a thread switch by activating the new thread's page
   tables, and, if the previous thread is dying, destroying it.

   At this function's invocation, we just switched from thread
   PREV, the new thread is already running, and interrupts are
   still disabled.  This function is normally invoked by
   thread_schedule() as its final action before returning, but
   the first time a thread is scheduled it is called by
   switch_entry() (see switch.S).

   It's not safe to call printf() until the thread switch is
   complete.  In practice that means that printf()s should be
   added at the end of the function.

   After this function and its caller returns, the thread switch
   is complete. */
void
thread_schedule_tail (struct thread *prev)
{
  struct thread *cur = running_thread ();
  
  ASSERT (intr_get_level () == INTR_OFF);

  /* Mark us as running. */
  cur->status = THREAD_RUNNING;

  /* Start new time slice. */
  thread_ticks = 0;

#ifdef USERPROG
  /* Activate the new address space. */
  process_activate ();
#endif

  /* If the thread we switched from is dying, destroy its struct
     thread.  This must happen late so that thread_exit() doesn't
     pull out the rug under itself.  (We don't free
     initial_thread because its memory was not obtained via
     palloc().) */
  if (prev != NULL && prev->status == THREAD_DYING && prev != initial_thread) 
    {
      ASSERT (prev != cur);
      palloc_free_page (prev);
    }
}

/* Schedules a new process.  At entry, interrupts must be off and
   the running process's state must have been changed from
   running to some other state.  This function finds another
   thread to run and switches to it.

   It's not safe to call printf() until thread_schedule_tail()
   has completed. */
static void
schedule (void) 
{
  struct thread *cur = running_thread ();
  struct thread *next = next_thread_to_run ();
  struct thread *prev = NULL;

  ASSERT (intr_get_level () == INTR_OFF);
  ASSERT (cur->status != THREAD_RUNNING);
  ASSERT (is_thread (next));

  if (cur != next)
    prev = switch_threads (cur, next);
  thread_schedule_tail (prev);
}

/* Returns a tid to use for a new thread. */
static tid_t
allocate_tid (void) 
{
  static tid_t next_tid = 1;
  tid_t tid;

  lock_acquire (&tid_lock);
  tid = next_tid++;
  lock_release (&tid_lock);

  return tid;
}

/* Offset of `stack' member within `struct thread'.
   Used by switch.S, which can't figure it out on its own. */
uint32_t thread_stack_ofs = offsetof (struct thread, stack);

/* Start Added Context of Project 1 */
#ifdef DEBUGTHREAD
struct thread* getParent(struct thread* t)
{
  return list_entry (t->elem_parent, struct thread, elem_child);
}
#endif
struct thread* getChild_byElem(struct list_elem* elem)
{
  return list_entry (elem, struct thread, elem_child);
}

struct thread* getChild_byList(struct thread* t, tid_t tid)
{ 
  struct list* childlist = &(t->list_child);
  struct list_elem* childelem;
  struct thread* child;
  for (childelem = list_begin (childlist); childelem != list_end (childlist);
  childelem = list_next (childelem))
  {
    child = getChild_byElem(childelem);
    if(child->tid == tid)
    {
      list_remove(childelem);
      return child;
    }
  }
  return NULL;
}

struct thread* getChild_byList_nonremove(struct thread* t, tid_t tid)
{ 
  struct list* childlist = &(t->list_child);
  struct list_elem* childelem;
  struct thread* child;
  for (childelem = list_begin (childlist); childelem != list_end (childlist);
  childelem = list_next (childelem))
  {
    child = getChild_byElem(childelem);
    if(child->tid == tid)
      return child;
  }
  return NULL;
}
/* End Added Context of Project 1 */

/* Start Added COntext of Project 2 */
struct thread* getThread_byElem(struct list_elem * elem)
{
  return list_entry (elem, struct thread, elem);
}
static struct thread* getThread_byElem_const(const struct list_elem * elem)
{
  return list_entry (elem, struct thread, elem);
}

bool less_thread_endsleep(const struct list_elem* a, const struct list_elem* b, void* aux)
{
  ASSERT(aux==NULL);

  if(getThread_byElem_const(a)->end_sleep <= getThread_byElem_const(b)->end_sleep)
    return true;
  return false;
}
bool greater_thread_priority(const struct list_elem* a, const struct list_elem* b, void* aux)
{
  ASSERT(aux==NULL);

  if(getThread_byElem_const(a)->priority > getThread_byElem_const(b)->priority)
    return true;
  return false;
}


void thread_sleep(int64_t end)
{
   //현재 list를 재운다.
  struct thread *sleep_thread;
    
  sleep_thread = thread_current();

  //Unless the system is otherwise idle,
  ASSERT (sleep_thread  != idle_thread);

  //sleeplist에 넣는다.
  
  //critical section start
  enum intr_level old_level;
  old_level = intr_disable ();

  //재울 리스트를 재운다.
  sleep_thread->end_sleep = end;

  //list는 빨리 끝나는 순서대로 정렬된다.
  list_insert_ordered(&sleep_list,&(sleep_thread->elem),less_thread_endsleep,NULL);

  thread_block();

  intr_set_level (old_level);
  //crictical section end

}

void thread_awake(void)
{
  struct thread *wake_thread = NULL;
  // sleep_list에 있는 것 중, 기다릴 만큼 기다린 것들은 빠진다.
  // list 비어있으면 돌아갈 필요가 없다.
  while(!list_empty(&sleep_list))
  {
    wake_thread = getThread_byElem(list_front(&sleep_list));
    ASSERT(is_thread(wake_thread));
    // 맨 앞에 있는 것이 제일 빨리 나갈 때인지라
    // 맨 앞의 있는 쓰레드가 아직 나갈 때가 아니면 뒤에 있는 것도 다 나갈 때가 아니니 멈춘다.
    if(timer_ticks() < wake_thread->end_sleep)
      break;
    // 나갈 때가 되면, block을 풀어준다.
    else
    { 
      if(wake_thread==NULL)
        return;
      list_pop_front(&sleep_list);
      thread_unblock(wake_thread);  
    }
  }
  if(intr_context()==false && next_thread()->priority > thread_current()->priority)
  {
    thread_yield ();
  }
}
bool priority_allupdate(void)
{
  struct list_elem* e;
  struct thread* t, *current = thread_current();

    for(e=list_begin(&all_list);
     e!=list_end(&all_list);
      e=list_next(e))
      {
         t = list_entry(e, struct thread, allelem);
        priority_update(t);
      }
  
  //printReadythread();

  if(current->priority < next_thread()->priority)
  {
        return true;
  }
  return false;
}

void priority_update(struct thread* t)
{
  //priority = PRI_MAX - (recent_cpu / 4) - (nice * 2),
  if(t==NULL || t==idle_thread)
    return;
  int temp1, temp2, temp3, temp4; //all fixed
  temp1 = tofixed(PRI_MAX);
  temp2 = divcompound(t->recent_cpu, 4);
  temp3 = multcompound(tofixed(t->nice),2);
  temp1 = subfixed(temp1, temp2);
  temp1 = subfixed(temp1, temp3);
  temp1 = toint(temp1);

  temp4 = t->priority;
  t->priority = temp1;
  
  if(t->priority > PRI_MAX)
    t->priority = PRI_MAX;
  if(t->priority < PRI_MIN)
    t->priority = PRI_MIN;

  if(temp4 != t->priority && t!= running_thread()
    && t->status !=THREAD_BLOCKED){
    list_remove(&t->elem);
    list_push_back (ready_list+(t->priority), &t->elem);
  }
}
void recent_cpu_allupdate(void)
{
  struct list_elem* e;
  struct thread *t;

  //if(thread_current() != idle_thread)
  //미안 재혁아 아까 트롤로 말함 ㅎ idle이라도 해야됨 
    for (e = list_begin(&all_list);
         e != list_end(&all_list);
         e = list_next(e))
    {
      t = list_entry(e, struct thread, allelem);
      recent_cpu_update(t);
    }

}
void recent_cpu_update(struct thread *t)
{
  /*fixed*/int dload_avg = multcompound(load_avg,2);
  int temp2 = addcompound(dload_avg,1);
  int temp3 = tofixed(t->nice);

  dload_avg = divfixed(dload_avg,temp2);
  dload_avg = multfixed(dload_avg, t->recent_cpu);
  dload_avg = addfixed(dload_avg,temp3);
  t->recent_cpu = dload_avg;  
}
void recent_cpu_increment(void)
{
  if (thread_current () == idle_thread)
    return;
  thread_current ()->recent_cpu = addcompound(thread_current ()->recent_cpu,1);
}
void load_avg_update(void)
{
  int ready_thread = 0;
  int i;
  for(i=0;i<=PRI_MAX;i++)
    ready_thread += list_size(ready_list+i);

  if(thread_current() != idle_thread)
    ready_thread+=1;


  //printf("\n==cur_avg : %d , ready_thread : %d, ", load_avg, ready_thread);

  int most = divfixed(tofixed(59),tofixed(60));
  int least = divfixed(tofixed(1),tofixed(60));
  most = multfixed(most, load_avg);
  least = multfixed(least, tofixed(ready_thread));
  load_avg = addfixed(most, least); 

  //printf("calc_result : %d==\n", thread_get_load_avg());
}

struct thread *
next_thread (void) 
{
  int i;
  for(i=PRI_MAX;i>=PRI_MIN;i--)
  {
    if(!list_empty(ready_list+i))
      return list_entry (list_front (ready_list+i), struct thread, elem);
  }
  return idle_thread;
}

void printReadythread(void)
{
  int i;
  int ready_count = 0;
  struct list_elem* e;

  printf("\nload average : %d, ",thread_get_load_avg());
  printf("current: ");
  printf("<%s(%d)> , ", thread_current()->name, thread_get_recent_cpu());


  for(i=0;i<=PRI_MAX;i++)
  {
    printf("prioirty(%d) : ",i);
    for(e = list_begin(ready_list+i); e != list_end(ready_list+i); e = list_next(e))
    {
      printf("<%s(%d)> ",getThread_byElem(e)->name, 
        toint_r(
          multcompound(
            getThread_byElem(e)->recent_cpu,
            100
            )
          )
      );
      ready_count++;
    }
    printf("\n");
  }
  printf("ready count : %d\n",ready_count);

  printf("sleep : ");
  for(e= list_begin(&sleep_list); e != list_end(&sleep_list);e=list_next(e))
  {
    printf("<%s(%d)> ",
    getThread_byElem(e)->name, 
    toint_r(
          multcompound(
            getThread_byElem(e)->recent_cpu,
            100
            )
          )
    );
  }
  printf("\n");
  
}
/* End Added COntext of Project 2 */
