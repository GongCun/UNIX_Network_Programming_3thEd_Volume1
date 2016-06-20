#ifndef _ADVUDP_H
#define _ADVUDP_H

#include "unp.h"

extern FILE *fpseq;
/* for record the recv sequence# */

ssize_t 
Adv_recvfrom_flags(int fd, void *ptr, size_t nbytes, int *flagsp,
        SA *sa, socklen_t *salenptr, struct unp_in_pktinfo *pktp); /* lib/unp.h */

void
adv_dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen);

ssize_t Adv_dg_send_recv(int, const void *, size_t, void *, size_t,
        const SA *, socklen_t);

void adv_dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen);

#endif
