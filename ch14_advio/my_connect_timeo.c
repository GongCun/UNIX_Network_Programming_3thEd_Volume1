#include "myadvio.h"
#include <string.h>

static void sig_alrm(int);

int my_connect_timeo(int sockfd, const SA *saptr, socklen_t salen, long sec, long usec)
{
    Sigfunc *sigfunc;
    int ret;
    struct itimerval delay, odelay;
    delay.it_value.tv_sec = sec;
    delay.it_value.tv_usec = usec;
    delay.it_interval.tv_sec = 0;
    delay.it_interval.tv_usec = 0;

    sigfunc = Signal(SIGALRM, sig_alrm);
    if (setitimer(ITIMER_REAL, &delay, &odelay) == -1)
        err_sys("setitimer error");

    if ((ret = connect(sockfd, saptr, salen)) < 0) {
        close(sockfd);
        if (errno == EINTR)
            errno = ETIMEDOUT;
    }
    if (setitimer(ITIMER_REAL, &odelay, NULL) == -1)
        err_sys("setitimer error");
    Signal(SIGALRM, sigfunc); /* restore previous signal handler */

    return(ret);

}

static void sig_alrm(int signo)
{
    return; /* just interrupt the connect() */
}
