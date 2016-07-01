#include "mynames.h"
#include <sys/time.h>

#define ITIMER_REAL      0
#define ITIMER_VIRTUAL   1
#define ITIMER_PROF      2

static void sig_alarm(int signo)
{
    printf("\ncaught SIGALRM\n");
}
    

int main(int argc, char **argv)
{
    char host[MAXLINE], serv[32];
    char *ptr;
    struct sockaddr_in *saptr;
    int ret;
    struct itimerval delay;

    if (signal(SIGALRM, sig_alarm) == SIG_ERR)
        err_sys("signal error");
    delay.it_value.tv_sec = 0;
    delay.it_value.tv_usec = 100;
    delay.it_interval.tv_sec = 0;
    delay.it_interval.tv_usec = 0;
    ret = setitimer(ITIMER_REAL, &delay, NULL);
    if (ret)
        err_sys("setitimer error");

    saptr = malloc(sizeof(struct sockaddr_in));
    if (saptr == NULL)
        err_sys("malloc error");
    
    while (--argc > 0) {
        ptr = *++argv;
        bzero(saptr, sizeof(struct sockaddr_in));
        saptr->sin_family = AF_INET;
        if (inet_aton(ptr, &saptr->sin_addr) == 0) {
            err_msg("inet_aton error: %s", ptr);
            continue;
        }

        ret = getnameinfo((SA *)saptr, sizeof(struct sockaddr_in),
                host, sizeof(host),
                serv, 0, NI_NAMEREQD|NI_NOFQDN);
        if (ret != 0) {
            err_msg("getnameinfo error for host: %s: %s", ptr, gai_strerror(ret));
            continue;
        }
        printf("official hostname: %s\n", host);
    }

    free(saptr);
    exit(0);

}

