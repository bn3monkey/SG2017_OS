#include "userprog/syscall.h"
#include <stdio.h>
#include "threads/interrupt.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "devices/input.h"
#include "threads/vaddr.h"
#include "userprog/process.h"
#include <syscall-nr.h>


static void syscall_handler (struct intr_frame *);

typedef void sys_func(struct intr_frame *f UNUSED);
sys_func* sys_func_arr[SYS_INVALID + 1];

/* For using system call */
 
//For Original System call
/* Projects 2 and later. */
void s_halt (void) NO_RETURN;
//For using this function external
//void s_exit (int status) NO_RETURN;
pid_t s_exec (const char *file);
int s_wait (pid_t);
bool s_create (const char *file, unsigned initial_size);
bool s_remove (const char *file);
int s_open (const char *file);
int s_filesize (int fd);
int s_read (int fd, void *buffer, unsigned length);
int s_write (int fd, const void *buffer, unsigned length);
void s_seek (int fd, unsigned position);
unsigned s_tell (int fd);
void s_close (int fd);
int s_fibonacci (int n);
int s_sum_of_four_integers (int a, int b, int c, int d);

//For using the array of system call
static void sys_func_halt(struct intr_frame *f UNUSED);
static void sys_func_exit (struct intr_frame *f UNUSED);
static void sys_func_exec (struct intr_frame *f UNUSED);
static void sys_func_wait (struct intr_frame *f UNUSED);
static void sys_func_create (struct intr_frame *f UNUSED);
static void sys_func_remove (struct intr_frame *f UNUSED);
static void sys_func_open (struct intr_frame *f UNUSED);
static void sys_func_filesize (struct intr_frame *f UNUSED);
static void sys_func_read (struct intr_frame *f UNUSED);
static void sys_func_write (struct intr_frame *f UNUSED);
static void sys_func_seek (struct intr_frame *f UNUSED);
static void sys_func_tell (struct intr_frame *f UNUSED);
static void sys_func_close (struct intr_frame *f UNUSED);
static void sys_func_fibonacci (struct intr_frame *f UNUSED);
static void sys_func_sum_of_four_integers (struct intr_frame *f UNUSED);
static void sys_func_invalid (struct intr_frame *f UNUSED);

//For using parameter
static int sys_func_name(struct intr_frame *f UNUSED);
static int sys_arg_int(int arg, struct intr_frame *f UNUSED);
static int sys_arg_int(int arg, struct intr_frame *f UNUSED);

//For lock
void sys_lock_init(void);
void sys_lock_acquire(void);
void sys_lock_release(void);
void sys_lock_force_release(void);
void force_exit(void);

/* For system lock */

struct lock sys_lock;
//For locking when when system call is called twice.
void sys_lock_init(void)
{
  lock_init(&sys_lock);
}
void sys_lock_acquire(void)
{
  lock_acquire(&sys_lock);
}
void sys_lock_release(void)
{
  lock_release(&sys_lock);
}
//For using to make system calling twice such as halt or exit available.  
void sys_lock_force_release(void)
{
  if(lock_held_by_current_thread(&sys_lock))
    sys_lock_release();
}

/* For Validating User Memory Access */
//if there is invalid value on pointer or value, the thread calling system_call exits
void force_exit(void)
{
  sys_lock_force_release();
  s_exit(-1);
}
static void validate_user_vaddr(void* p)
{
  //if pointer is NULL, it's ERROR!
  if(p == NULL)
  {
    #ifdef DEBUGARG
    printf("the pointer lack : %p\n", p);
    #endif
    force_exit();
  }
  //if pointer is kernel_vaddr, it's ERROR!
  if(is_kernel_vaddr(p))
  {
    #ifdef DEBUGARG
    printf("the pointer lack : %p\n", p);
    #endif
    force_exit();
  }
}  


/* For getting argument */

enum sys_arg_enum
{
  FUNC_NAME,
  FUNC_ARG1,
  FUNC_ARG2,
  FUNC_ARG3,
  FUNC_ARG4,
};
static size_t sys_arg_gap(int num)
{
  return num * sizeof(void *);
}
//Only can be used in the function which intr_frame *f is a paremeter of 

//For using parameter
#define sys_in(SYS_ARG_NUM, TYPE) (*(TYPE *)(f->esp+sys_arg_gap(SYS_ARG_NUM)))
//For Getting func_name
static int sys_func_name(struct intr_frame *f UNUSED)
{
  //Validity Check of Stack Pointer
  validate_user_vaddr(f->esp);

  int temp = sys_in(FUNC_NAME, int);

  //Validity Check of Value.
  #ifdef DEBUGARG
  printf("the sytem call name is %d\n", temp);
  #endif

  if(temp<0 || temp>=SYS_INVALID)
    temp = SYS_INVALID;

  return temp;
}
//For Getting integer parameter
static int sys_arg_int(int arg, struct intr_frame *f UNUSED)
{
  //Validity Check of Stack Pointer
  validate_user_vaddr(f->esp+sys_arg_gap(arg));

  int temp = sys_in(arg, int);
  
  //Validity Check of Value.
  #ifdef DEBUGARG
  printf("the sytem call argument(%d) is %d\n", arg, temp);
  #endif
  //Nothing

  return temp;
}
//For Getting String parameter
static char* sys_arg_str(int arg, struct intr_frame *f UNUSED)
{
  //Validity Check of Stack Pointer
  validate_user_vaddr(f->esp+sys_arg_gap(arg));

  char* temp = sys_in(arg, char *);
  #ifdef DEBUGARG
  printf("the sytem call argument(%d) is %s\n", arg, temp);
  #endif
  //Validity Check of Value.
  if(temp == NULL)
    force_exit();

  return temp;
}

//For using return
#define sys_out(FUNC) f->eax=(unsigned int)(FUNC)


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");

  sys_lock_init();

  sys_func_arr[SYS_HALT] = sys_func_halt;
  sys_func_arr[SYS_EXIT] = sys_func_exit;
  sys_func_arr[SYS_EXEC] = sys_func_exec;
  sys_func_arr[SYS_WAIT] = sys_func_wait;
  sys_func_arr[SYS_CREATE] = sys_func_create;
  sys_func_arr[SYS_REMOVE] = sys_func_remove;
  sys_func_arr[SYS_OPEN] = sys_func_open;
  sys_func_arr[SYS_FILESIZE] = sys_func_filesize;
  sys_func_arr[SYS_READ] = sys_func_read;
  sys_func_arr[SYS_WRITE] = sys_func_write;
  sys_func_arr[SYS_SEEK] = sys_func_seek;
  sys_func_arr[SYS_TELL] = sys_func_tell;
  sys_func_arr[SYS_CLOSE] = sys_func_close;
  sys_func_arr[SYS_FIBONACCI] = sys_func_fibonacci;
  sys_func_arr[SYS_SUM_OF_FOR_INTEGERS] = sys_func_sum_of_four_integers;
  sys_func_arr[SYS_INVALID] = sys_func_invalid;
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  sys_func_arr[sys_func_name(f)] (f);
}

static void sys_func_halt(struct intr_frame *f UNUSED)
{
  sys_lock_force_release();
  s_halt();
  NOT_REACHED();
}
static void sys_func_exit (struct intr_frame *f UNUSED)
{
  sys_lock_force_release();
  s_exit(
      sys_arg_int(FUNC_ARG1, f)
    );
  NOT_REACHED();
}
static void sys_func_exec (struct intr_frame *f UNUSED)
{
  sys_out(
      s_exec(
          sys_arg_str(FUNC_ARG1, f)
        )
      );
}
static void sys_func_wait (struct intr_frame *f UNUSED)
{
  sys_out(
    s_wait(
      sys_arg_int(FUNC_ARG1, f)
    )
  );
}
static void sys_func_create (struct intr_frame *f UNUSED)
{
  //Make Later.
}
static void sys_func_remove (struct intr_frame *f UNUSED)
{
  //Make Later.
}
static void sys_func_open (struct intr_frame *f UNUSED)
{
  //Make Later.
}
static void sys_func_filesize (struct intr_frame *f UNUSED)
{
  //Make Later.
}
static void sys_func_read (struct intr_frame *f UNUSED)
{
  sys_lock_acquire();
  sys_out(
    s_read(
      sys_arg_int(FUNC_ARG1, f),
      sys_arg_str(FUNC_ARG2, f),
      (unsigned)sys_arg_int(FUNC_ARG3, f)
    )
  );
  sys_lock_release();
}

static void sys_func_write (struct intr_frame *f UNUSED)
{
  sys_out(
    s_write(
      sys_arg_int(FUNC_ARG1, f),
      sys_arg_str(FUNC_ARG2, f),
      (unsigned)sys_arg_int(FUNC_ARG3, f)
    )
  );
}
static void sys_func_seek (struct intr_frame *f UNUSED)
{
  //Make Later.
}

static void sys_func_tell (struct intr_frame *f UNUSED)
{
  //Make Later.
}

static void sys_func_close (struct intr_frame *f UNUSED)
{
  //Make Later.
}

static void sys_func_fibonacci (struct intr_frame *f UNUSED)
{
  sys_out(
    s_fibonacci(
      sys_arg_int(FUNC_ARG1, f)
    )
  );
}
static void sys_func_sum_of_four_integers (struct intr_frame *f UNUSED)
{
  sys_out(
    s_sum_of_four_integers(
      sys_arg_int(FUNC_ARG1, f),
      sys_arg_int(FUNC_ARG2, f),
      sys_arg_int(FUNC_ARG3, f),
      sys_arg_int(FUNC_ARG4, f)
    )
  );
}
static void sys_func_invalid (struct intr_frame *f UNUSED)
{
  //Do Nothing
}

void s_halt (void)
{
  shutdown_power_off ();
}
void s_exit (int status)
{
  struct thread* current = thread_current();
  current->exit_status = status;
  printf ("%s: exit(%d)\n", current->name, current->exit_status);
  thread_exit();
}

pid_t s_exec (const char *file)
{
  return process_execute(file);
}
int s_wait (pid_t tid)
{
  return process_wait (tid);
}
/*
static bool s_create (const char *file, unsigned initial_size)
{
  //Make Later.
}
static bool s_remove (const char *file)
{
  //Make Later.
}
static int s_open (const char *file)
{
  //Make Later.
}
static int s_filesize (int fd)
{
  //Make Later.
}
*/
int s_read (int fd, void *buffer, unsigned length)
{
  unsigned i;
  if (fd == 0)
  {
    // 표준 입력
    for(i=0; i<length; i++)
      ((char *)buffer)[i] = input_getc();
  }
  return length;
}
int s_write (int fd, const void *buffer, unsigned length)
{
  if(fd == 1)
  {
    putbuf(buffer, length);
  }
  return length;
}
/*
static void s_seek (int fd, unsigned position)
{
  //Make Later.
}
static unsigned s_tell (int fd)
{
  //Make Later.
}
static void s_close (int fd)
{
  //Make Later.
}
*/
int s_fibonacci (int n)
{
  int sign;
  int a1 = 1;
  int a2 = 1;
  int t = 0;
  if(n==0)
    return 0;
  if(n<0)
    sign = -1;
  else
    sign = 1;

  n -= 2;
  while(n--)
  {
    t = a2;
    a2 += a1;
    a1 = t;
  }
  return sign * a2; 
}
int s_sum_of_four_integers (int a, int b, int c, int d)
{
  return a+b+c+d;
}
