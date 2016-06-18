#include "modeserv.h"
#include <sys/mman.h>

#if ((_PREFORK) || (_PASSFD))
extern int nchildren;
extern pid_t *ppids;
extern long *pservice;
extern struct child *cptr;
#if(__DYNAMIC)
extern volatile sig_atomic_t quitflag;
#endif
#endif

#if (_PRETHREAD)
extern struct Thread *pThread;
extern int nthreads;
#endif

void sig_int(int signo)
{
    void mode_cpu_time(void);
    int i;
#if (_PREFORK)
    for (i = 0; i < nchildren; i++) {
#if (_DEBUG)
        printf("pid[i] = %ld\n", (long) ppids[i]);
#endif
        kill(ppids[i], SIGTERM);
        printf(">>> pid %ld: %ld\n", (long)ppids[i], pservice[i]);
    }
    while (wait(NULL) > 0)
        ;
    if (errno != ECHILD)
        err_sys("wait error");
#elif (_PASSFD)
#if(__DYNAMIC)
    quitflag = 1; /* must before kill */
#endif
    for (i = 0; i< nchildren; i++) {
        kill(cptr[i].child_pid, SIGTERM);
        printf(">>> pid %ld: %ld\n", (long)cptr[i].child_pid, cptr[i].child_count);
    }
    while (wait(NULL) > 0)
        ;
    if (errno != ECHILD)
        err_sys("wait error");
#elif (_PRETHREAD)
    for (i = 0; i < nthreads; i++)
        printf(">>> tid %ld: %ld\n", (long)pThread[i].tid, (long)pThread[i].count);
#endif
    mode_cpu_time();
#if (_PREFORK)
    if (munmap(pservice, sizeof(long) * nchildren) < 0)
        err_sys("munmap error");
#endif

    exit(0);
}

