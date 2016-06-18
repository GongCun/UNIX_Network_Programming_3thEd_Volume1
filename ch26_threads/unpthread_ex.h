/* Our own header for the programs that use threads.
   Include this file, instead of "unp.h". */

#ifndef	__unp_pthread_ex_h
#define	__unp_pthread_ex_h

#include	"unpthread.h"

void thread_str_cli(FILE *, int);
void *thread_copyto(void *);

#endif	/* __unp_pthread_ex_h */
