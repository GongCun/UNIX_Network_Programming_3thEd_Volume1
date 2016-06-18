#include "udpcksum-ex.h"

int rawfd;

void dlt_open_raw(void)
{
    const int on = 1;

    if ((rawfd = socket(AF_INET, SOCK_RAW, 0)) < 0)
            err_sys("socket error");
    if (setsockopt(rawfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)
        err_sys("setsockopt error");
}

