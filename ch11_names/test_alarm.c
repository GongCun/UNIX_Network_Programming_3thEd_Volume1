#include "mynames.h"

static void sig_alrm(int signo)
{
    printf("caught SIGALRM\n");
}

int main(void)
{
    if (signal(SIGALRM, sig_alrm) == SIG_ERR)
        err_sys("signal error");
    alarm(5);
    fgetc(stdin);
    if (ferror(stdin) && errno != EINTR)
        err_sys("fgets error");
    alarm(0);

    printf("return\n");
    exit(0);

}

