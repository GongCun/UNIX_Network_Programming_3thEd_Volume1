#include "unpthread_ex.h"
#include "thread_readline.h"

static int sockfd; /* global for both threads to access */
static FILE *fp;
static int stdineof = 0;

void thread_str_cli(FILE *fp_arg, int sockfd_arg)
{
    char recvline[MAXLINE];
    pthread_t tid;

    sockfd = sockfd_arg;
    fp = fp_arg;

    Pthread_create(&tid, NULL, thread_copyto, NULL);

    while (Thread_readline(sockfd, recvline, MAXLINE) > 0)
        Fputs(recvline, stdout);
    thread_readlineflush(STDOUT_FILENO);

    if (stdineof == 0)
        err_quit("thread_str_cli: server terminated permaturely");
}

void *thread_copyto(void *arg)
{
    char sendline[MAXLINE];

    while (Fgets(sendline, MAXLINE, fp) != NULL)
        Writen(sockfd, sendline, strlen(sendline));

    Shutdown(sockfd, SHUT_WR); /* EOF on stdin, send FIN */
    stdineof = 1;

    return NULL;

}


