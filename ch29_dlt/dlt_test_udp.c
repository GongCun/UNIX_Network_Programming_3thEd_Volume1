#include "udpcksum-ex.h"
#include <setjmp.h>

static int canjmp = 0;
static jmp_buf jmpbuf;

/* sends queries and reads responses */
void dlt_test_udp(void)
{
    volatile int isent = 0, timeout = 3;

    if (signal(SIGALRM, dlt_sigalrm) == SIG_ERR)
        err_sys("signal error");

    if (sigsetjmp(jmpbuf, 1)) {
        if (isent >= 3)
            err_quit("no responses");
        fprintf(stderr, "timeout\n");
        timeout *= 2; /* 3, 6, 12 */
    }
    canjmp = 1;
    dlt_send_dnsquery();
    isent++;

    alarm(timeout);

    struct udpiphdr *pui; /* <netinet/udp_var.h>
                              UDP kernel structures and variables */
    pui = dlt_udp_read();
    canjmp = 0;
    alarm(0);

    if (pui->ui_sum == 0) /* #define ui_sum ui_u.uh_sum */
        printf("UDP checksums off\n");
    else
        printf("UDP checksums on\n");

    printf("received UDP checksum = 0x%x\n", ntohs(pui->ui_sum));
}

void dlt_sigalrm(int signo)
{
    if(canjmp == 0)
        return;
    siglongjmp(jmpbuf, 1);
}
