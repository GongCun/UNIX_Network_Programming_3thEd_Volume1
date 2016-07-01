#include "pracudp.h"

#define NDG 2000
#define DGLEN 1400

void exer07_dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    int i, ret;
    char sendline[DGLEN];
    struct timespec req = { .tv_sec = 0, .tv_nsec = 250000000 }, rem;

    for (i = 0; i < NDG; i++) {
        Sendto(sockfd, sendline, DGLEN, 0, pservaddr, servlen);
retry:
        ret = nanosleep(&req, &rem);
        if (ret) {
            if (errno == EINTR) {
                req.tv_sec = rem.tv_sec;
                req.tv_nsec = rem.tv_nsec;
                goto retry;
            }
            err_sys("nanosleep error");
        }
        printf("i = %d\n", i);
    }
}


