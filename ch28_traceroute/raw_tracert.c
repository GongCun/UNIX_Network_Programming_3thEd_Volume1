#include "raw_tracert.h"
#include <setjmp.h>

void 
raw_traceloop(void)
{
	if ((recvfd = socket(prp->psaSend->sa_family, SOCK_RAW, IPPROTO_ICMP)) < 0)
		err_sys("socket error");
	setuid(getuid());	/* don't need root permission anymore */

	if ((sendfd = socket(prp->psaSend->sa_family, SOCK_DGRAM, 0)) < 0)
		err_sys("socket error");
	bzero(prp->psaBind, sizeof(struct sockaddr));
	prp->psaBind->sa_family = prp->psaSend->sa_family;	/* AF_INET */
	sport = (pid & 0xffff) | 0x8000;	/* our souce UDP port */
	((struct sockaddr_in *)prp->psaBind)->sin_port = htons(sport);
	if (bind(sendfd, prp->psaBind, sizeof(struct sockaddr)) < 0)
		err_sys("bind error");

	int seq = 0, done = 0, probe = 0, code = 0;
	double rtt;
	struct rawrec *prawrec;
	struct timeval tvrecv;
	char strAddr[64], numAddr[64];
	for (ttl = 1; ttl <= max_ttl && done == 0; ttl++) {
		if (setsockopt(sendfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) < 0)
			err_sys("setsockopt error");
		bzero(prp->psaLast, sizeof(struct sockaddr));
		printf("%2d ", ttl);
		fflush(stdout);

		for (probe = 0; probe < nprobes; probe++) {
			prawrec = (struct rawrec *)sendbuf;
			prawrec->rawrec_seq = ++seq;
			prawrec->rawrec_ttl = ttl;
			if (gettimeofday(&prawrec->rawrec_tv, NULL) < 0)
				err_sys("gettimeofday error");
			((struct sockaddr_in *)prp->psaSend)->sin_port = htons(dport + seq);
			if (sendto(sendfd, sendbuf, sizeof(struct rawrec), 0, prp->psaSend, prp->slLen) != sizeof(struct rawrec))
				/* sizeof(struct rawrec) == datalen */
				err_sys("sendto error");
			if ((code = raw_recv(seq, &tvrecv)) == -3)
				printf(" *");	/* timeout, no reply */
			else {
				if (memcmp(&((struct sockaddr_in *)prp->psaRecv)->sin_addr,
					   &((struct sockaddr_in *)prp->psaLast)->sin_addr,
					   sizeof(struct in_addr)) != 0) {	/* not the same */
                    if (probe)
                        printf("\n   ");
					if (inet_ntop(AF_INET, &((struct sockaddr_in *)prp->psaRecv)->sin_addr, numAddr, sizeof(numAddr)) == NULL)
						err_sys("inet_ntop error");
                    if (nameconvert) {
					    if (getnameinfo(prp->psaRecv, prp->slLen, strAddr, sizeof(strAddr), NULL, 0, 0) == 0)
					    	printf(" %s (%s)", strAddr, numAddr);
                        else
                            printf(" %s", numAddr);
                    } else
                        printf(" %s", numAddr);
                    memcpy(prp->psaLast, prp->psaRecv, prp->slLen);
				}
				raw_tv_sub(&tvrecv, &prawrec->rawrec_tv);
				rtt = tvrecv.tv_sec * 1000.0 + tvrecv.tv_usec / 1000.0;
				printf(" %.3f ms", rtt);

				if (code == -1)	/* port unreachable: at
						 * destination */
					done++;
				else if (code >= 0)
					printf(" (ICMP %s)", raw_icmpcode(code));
			}	/* end of reply */
			fflush(stdout);
		}		/* end of nprobes */
		printf("\n");	/* end of ttl */
	}
}

void 
raw_sig_alrm(int signo)
{
    siglongjmp(gotalarm, 1); /* jump to raw_recv, code == -3 */
}


const char *
raw_icmpcode(int code)
{
	static char errbuf[100];
	switch (code) {
	case 0:
		return ("network unreachable");
	case 1:
		return ("host unreachable");
	case 2:
		return ("protocol unreachable");
	case 3:
		return ("port unreachable");
	case 4:
		return ("fragmentation required but DF bit set");
	case 5:
		return ("source route failed");
	case 6:
		return ("destination network unknown");
	case 7:
		return ("destination host unknown");
	case 8:
		return ("source host isolated (obsolete)");
	case 9:
		return ("destination network administratively prohibited");
	case 10:
		return ("destination host administratively prohibited");
	case 11:
		return ("network unreachable for TOS");
	case 12:
		return ("host unreachable for TOS");
	case 13:
		return ("communication administratively prohibited by filtering");
	case 14:
		return ("host recedence violation");
	case 15:
		return ("precedence cutoff in effect");
	default:
		sprintf(errbuf, "[unknown code %d]", code);
		return errbuf;
	}
}

int raw_recv(int seq, struct timeval *ptv)
{
    /*
     * Return:
     *   -3 on timeout
     *   -2 on ICMP time exceeded in transit (caller keep going)
     *   -1 on ICMP port unreachable (caller is done)
     *   >= 0 return value is come other ICMP unreachable code
     */
    struct ip *ip, *subip;
    struct icmp *icmp;
    struct udphdr *udp;
    socklen_t len;
    ssize_t n;
    int ret, iplen, subiplen, icmplen;
    char buf[64];

    alarm(waittime);
start_recv:
    if (sigsetjmp(gotalarm, 1))
        return -3; /* alarm expired */
    len = prp->slLen;
    if ((n = recvfrom(recvfd, recvbuf, sizeof(recvbuf), 0, prp->psaRecv, &len)) < 0) {
        if (errno == EINTR)
            goto start_recv;
        err_sys("recvfrom error");
    } /* n is total length */
    ip = (struct ip *)recvbuf; /* start of IP header */
    iplen = ip->ip_hl * 4;
    icmp = (struct icmp *)(recvbuf + iplen); /* start of ICMP header */
    if ((icmplen = n - iplen) < 8 + sizeof(struct ip)) /* not enough data to look at inner IP */
        goto start_recv;
    subip = (struct ip *)(recvbuf + iplen + 8);
    subiplen = subip->ip_hl * 4;
    if (icmplen < 8 + subiplen + sizeof(u_short) * 2) /* uh_sport + uh_dport = 2 * sizeof(u_short) */
        goto start_recv;
    udp = (struct udphdr *)(recvbuf + iplen + 8 + subiplen);
    if (icmp->icmp_type == ICMP_TIMXCEED &&
            icmp->icmp_code == ICMP_TIMXCEED_INTRANS) { /* type == 11, code == 0 */
        if (subip->ip_p == IPPROTO_UDP &&
                udp->uh_sport == htons(sport) &&
                udp->uh_dport == htons(dport+seq))
        {
            ret = -2; /* hit an intermediate router */
        } else 
            goto start_recv;
    } else if (icmp->icmp_type == ICMP_UNREACH) { /* type == 3 */
        if (subip->ip_p == IPPROTO_UDP &&
                udp->uh_sport == htons(sport) &&
                udp->uh_dport == htons(dport+seq)) {
            if (icmp->icmp_code == ICMP_UNREACH_PORT) { /* code == 3 */
                ret = -1;
            } else {
                ret = icmp->icmp_code;
                if (inet_ntop(AF_INET, &((struct sockaddr_in *)(prp->psaRecv))->sin_addr,
                            buf, sizeof(buf)) == NULL)
                    err_sys("inet_ntop error");
                printf(" (from %s: type = %d, code = %d)\n", buf, icmp->icmp_type, icmp->icmp_code);
            }
        } else
            goto start_recv;
    } else
        goto start_recv;
    alarm(0);
    if (gettimeofday(ptv, NULL) < 0)
        err_sys("gettimeofday error");
    return ret;
}


void raw_tv_sub(struct timeval *out, struct timeval *in)
{
    if ((out->tv_usec -= in->tv_usec) < 0) {
        --out->tv_sec;
        out->tv_usec += 1000000;
    }
    out->tv_sec -= in->tv_sec;
}



