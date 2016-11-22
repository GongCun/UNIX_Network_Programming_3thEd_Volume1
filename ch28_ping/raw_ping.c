#include "raw_ping.h"
#include <assert.h>

void raw_init(void)
{
    /* for IPv6 */
    return;
}

void raw_readloop(void)
{
        const int on = 1;

    /* the program must have superuser privileges to 
     * create the raw socket */
    sockfd = socket(prp->sasend->sa_family, SOCK_RAW, prp->icmpproto); /* AF_INET, SOCK_RAW, IPPROTO_ICMP */
    if (sockfd < 0)
        err_sys("socket error");

    if (raw_hdr)
            if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)
                    err_sys("setsockopt IP_HDRINCL error");

    recvfd = socket(prp->sasend->sa_family, SOCK_RAW, prp->icmpproto); /* AF_INET, SOCK_RAW, IPPROTO_ICMP */
    if (recvfd < 0)
            err_sys("socket error");

    /* no longer need superuser privileges */
    if (setuid(getuid()) < 0)
        err_quit("setuid error");

    if (prp->finit)
        (*prp->finit)(); /* IPv4 no need */

    int size;
    size = 60 * 1024; /* 61,440 bytes, which should be lager than
                         default, in case the user _pings_ either the
                         IPv4 broadcast address or a multicast address,
                         avoid overflow the recv buff */
    setsockopt(recvfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)); /* OK if set failed */

#ifdef _THREAD_TEST
    pthread_t tid;
    int errcode;
    if ((errcode = pthread_create(&tid, NULL, sendloop, NULL)) != 0)
        err_quit("pthread_create error: %s", strerror(errcode));
#ifdef _DEBUG
    printf("external thread id = %p\n", tid);
#endif

#else
    raw_sig_alrm(SIGALRM); /* send first packet */
#endif

    struct iovec iov;
    char recvbuf[BUFSIZE]; /* 1500 */
    iov.iov_base = recvbuf;
    iov.iov_len = sizeof(recvbuf);

    struct msghdr msg;
    char controlbuf[BUFSIZE];
    msg.msg_name = prp->sarecv;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = controlbuf; /* controlbuf is struct cmsghdr */

    ssize_t n;
    struct timeval tval;
    for (;;) {
        msg.msg_namelen = prp->salen;
        msg.msg_controllen = sizeof(controlbuf);
        if ((n = recvmsg(recvfd, &msg, 0)) < 0) {
            if (errno == EINTR)
                continue;
            err_sys("recvmsg error");
        }
        if (gettimeofday(&tval, NULL) < 0)
            err_sys("gettimeofday error");
        (*prp->fproc)(recvbuf, n, &msg, &tval);
    }
}


void raw_tv_sub(struct timeval *out, struct timeval *in)
{
    if ((out->tv_usec -= in->tv_usec) < 0) {
        --out->tv_sec;
        out->tv_usec += 1000000;
    }
    out->tv_sec -= in->tv_sec;
}

/* processes all received ICMPv4 messages */
void raw_proc(char *ptr, ssize_t len, struct msghdr *pmsg, struct timeval *tvrecv)
{
    struct ip *ip;
    ip = (struct ip *)ptr;
    
    int iplen;
    iplen = ip->ip_hl * 4; /* length of IP header, include any option */
    if (ip->ip_p != IPPROTO_ICMP)
        return; /* not ICMP */

    struct icmp *icmp;
    icmp = (struct icmp *)(ptr + iplen); /* start of ICMP header */

    int icmplen;
    if ((icmplen = len - iplen) < 8)
        return;

    if (icmp->icmp_type == ICMP_ECHOREPLY) {
        if (icmp->icmp_id != pid)
            return; /* not a response to our ECHO_REQUEST */
        if (icmplen < 16)
            return; /* not enough data to use */

        struct timeval *tvsend;
        double rtt;
        tvsend = (struct timeval *)icmp->icmp_data;
        raw_tv_sub(tvrecv, tvsend);
        rtt = tvrecv->tv_sec*1000.0 + tvrecv->tv_usec/1000.0;
        printf("%d bytes from %s: seq=%u, ttl=%d, rtt=%.3f ms\n",
                icmplen, strHost, icmp->icmp_seq, ip->ip_ttl, rtt);
    } else {
        printf("%d bytes from %s: type = %d, code = %d\n",
                icmplen, strHost, icmp->icmp_type, icmp->icmp_code);
    }
}

static uint16_t raw_in_cksum(uint16_t *addr, int len);

/* build an ICMPv4 echo request message and sends it */
void raw_send(void)
{
    struct icmp *icmp;
    icmp = (struct icmp *)sendbuf;
    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_code = 0;
    icmp->icmp_id = pid;
    icmp->icmp_seq = nsent++;
    if (gettimeofday((struct timeval *)icmp->icmp_data, NULL) < 0)
        err_sys("gettimeofday error");

    int i;
    char *datap;
    assert(datalen >= sizeof(struct timeval));
    datap = icmp->icmp_data + sizeof(struct timeval);
    /* pad out the length of message */
    for (i = 8 + sizeof(struct timeval); i < datalen; i++)
        *datap++ = i;

    int len;
    len = 8 + datalen; /* checksum ICMP header and data */
    icmp->icmp_cksum = 0;
    icmp->icmp_cksum = raw_in_cksum((uint16_t *) icmp, len);
    if (sendto(sockfd, sendbuf, len, 0, prp->sasend, prp->salen) < 0)
        err_sys("sendto error");
}

void 
raw_send_hdr(void)
{
#ifndef IP_OFFMASK
#define IP_OFFMASK 0x1fff
#endif
	struct icmp *icmp;
	struct ip *ip;

	int remlen = datalen + 8;
	int offunit = (xmtu - 20) / 8;
	int offsize = offunit * 8;
	static int count = 0;
	int loop = 0;
	int len;
	u_short id = htons((u_short) (time(0) + count++) & 0xffff);

        if (verbose)
                printf("MTU = %d, offset size = %d\n", xmtu, offsize);

	while (remlen > 0) {
#ifdef _DEBUG
                printf("loop = %d, remlen = %d\n", loop, remlen);
#endif
		ip = (struct ip *)sendbuf;
		ip->ip_hl = 5;
		ip->ip_v = 4;
		ip->ip_tos = 0;
#if defined(_LINUX) || defined (_OPENBSD)
		/* ip->ip_len = htons(20 + remlen); */
		ip->ip_len = htons(20 + min(remlen, offsize));
#else
		/* ip->ip_len = 20 + remlen; */
		ip->ip_len = 20 + min(remlen, offsize);
#endif

		ip->ip_id = id;

		if (remlen <= offsize) {
			if (loop == 0) {
#ifdef _LINUX
				ip->ip_off = htons(IP_DF);
#else
				ip->ip_off = IP_DF;
#endif
			} else { /* last packet */
				/* ip->ip_off = htons(loop * offunit); */
				/* ip->ip_off = htons(loop * offunit & IP_OFFMASK); */
#ifdef _LINUX
				ip->ip_off = htons(loop * offunit);
#else
				ip->ip_off = loop * offunit;
#endif
			}
		} else {
			if (loop == 0) {
#ifdef _LINUX
				ip->ip_off = htons(IP_MF);
#else
				ip->ip_off = IP_MF;
                                printf("MF\n");
#endif
			} else {
				ip->ip_off = htons(loop * offunit | IP_MF);
			}
		}

		ip->ip_ttl = 255;
		ip->ip_p = IPPROTO_ICMP;
		ip->ip_dst = ((struct sockaddr_in *)prp->sasend)->sin_addr;
		ip->ip_src = src;
		ip->ip_sum = 0;
		ip->ip_sum = raw_in_cksum((uint16_t *) ip, ip->ip_hl * 4);

		if (loop == 0) {

			icmp = (struct icmp *)(sendbuf + ip->ip_hl * 4);
			icmp->icmp_type = ICMP_ECHO;
			icmp->icmp_code = 0;
			icmp->icmp_id = pid;
			icmp->icmp_seq = nsent++;
			memset(icmp->icmp_data, 0xa5, datalen);
			if (gettimeofday((struct timeval *)icmp->icmp_data, NULL) < 0)
				err_sys("gettimeofday error");

			len = 8 + datalen;	/* checksum ICMP header and
						 * data */
			icmp->icmp_cksum = 0;
			icmp->icmp_cksum = raw_in_cksum((uint16_t *) icmp, len);
                        /* sleep(1000); */
                        printf("len = %d, offsize = %d\n", len, offsize);
			/* if (sendto(sockfd, sendbuf, 20 + (len < offsize ? len : offsize), 0, prp->sasend, prp->salen) < 0) */
			if (sendto(sockfd, sendbuf, 20 + offsize, 0, prp->sasend, prp->salen) < 0)
			/* if (sendto(sockfd, sendbuf, 20 + len, 0, prp->sasend, prp->salen) < 0) */
				err_sys("sendto error");
#ifdef _DEBUG
                        printf("sendto success\n");
#endif
		} else {
			memmove(sendbuf + 20, sendbuf + 20 + offsize, remlen);
			if (sendto(sockfd, sendbuf, 20 + remlen, 0, prp->sasend, prp->salen) < 0)
				err_sys("sendto error");
		}
		loop++;
		remlen -= offsize;
	}
}
void *sendloop(void *arg)
{
#if defined(_THREAD_TEST) && defined(_DEBUG)
    printf("thread id = %p\n", pthread_self());
#endif
    for (;;) {
            (*prp->fsend)();
#if defined(_THREAD_TEST) && defined(_DEBUG)
        printf("thread sent\n");
#endif
        sleep(1);
    }
    return NULL;
}

/* u_short icmp_cksum */
static uint16_t raw_in_cksum(uint16_t *addr, int len)
{
    int nleft = len;
    uint32_t sum = 0;
    uint16_t *w = addr;
    uint16_t answer = 0;

    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1) {
        *(unsigned char *)(&answer) = *(unsigned char *)w;
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return answer;
}

void raw_sig_alrm(int signo)
{
    (*prp->fsend)();
    alarm(1);
    return;
}
