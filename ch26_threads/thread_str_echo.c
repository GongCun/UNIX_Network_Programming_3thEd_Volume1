#include "unpthread_ex.h"
#include "thread_readline.h"


void thread_str_echo(int fd)
{
    char buf[MAXLINE];
    int n;

again:
    while ((n = Thread_readline(fd, buf, MAXLINE)) > 0)
        Writen(fd, buf, n);
    thread_readlineflush(fd);

    if (n < 0 && errno == EINTR)
        goto again;
    else if (n < 0)
        err_sys("thread_str_echo: thread_readline error");
}
