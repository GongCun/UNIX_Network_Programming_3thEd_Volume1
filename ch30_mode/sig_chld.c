#include "modeserv.h"

void sig_chld(int signo)
{
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
#if (_DEBUG)
        printf("process %ld terminated\n", (long)pid)
#endif
        ;

    if (pid < 0 && errno != ECHILD)
        err_sys("waitpid error");

    return;
}
