#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

/* Start Added Context of Project 1 */

#include <stdbool.h>
#include <debug.h>

/* Process identifier. */
typedef int pid_t;
#define PID_ERROR ((pid_t) -1)

/* Typical return values from main() and arguments to exit(). */
#define EXIT_SUCCESS 0          /* Successful execution. */
#define EXIT_FAILURE 1          /* Unsuccessful execution. */

/* For debugging argument */
//#define DEBUGARG

void syscall_init (void);

//For using this function external
void s_exit (int status) NO_RETURN;

/* End Added Context of Project 1 */

#endif /* userprog/syscall.h */
