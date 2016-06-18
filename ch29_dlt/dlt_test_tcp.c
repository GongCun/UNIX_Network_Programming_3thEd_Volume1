#include "unp.h"
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <libgen.h>
#include <pcap.h>
#if (AIX)
#include <net/bpf.h>
#endif
#include <assert.h>

#define SNAPLEN 1514 /* max packet size */
#define TO_MS 1000

char *device;
pcap_t *ppt;
int linktype;
int rawfd;

struct pseudo_tcphdr {
    struct in_addr ip_src, ip_dst;
    u_char pad, ip_p;
    u_short tcp_len;
    struct tcphdr tcphdr;
    u_char data[576]; /* guarantee the host will receive,
                         will not be truncated */
};

extern uint16_t dlt_cksum(uint16_t *addr, int len);

void dlt_tcp_push(const u_char *);
void dlt_tcp_fin(const u_char *);
void dlt_tcp_ack(const u_char *);
void dlt_tcp_pcap(struct sockaddr *);
void dlt_handler(u_char *, const struct pcap_pkthdr *, const u_char *);
void dlt_encap_ip(u_char *buf, int userlen, struct in_addr src, struct in_addr dst, u_short sport, u_short dport, u_char flags, uint32_t seq, uint32_t ack);

int main(int argc ,char *argv[])
{
    const int on = 1;
    char buf[576];

    if (argc != 5)
        err_quit("Usage: %s <srcIP> <host> <service> <device>", basename(argv[0]));

    device = argv[4];

    struct addrinfo *res, hint;
    int ret;
    struct sockaddr *servaddr;
    bzero(&hint, sizeof(struct addrinfo));
    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    if ((ret = getaddrinfo(argv[2], argv[3], &hint, &res)) != 0)
        err_quit("getaddrinfo error: %s", gai_strerror(ret));

    servaddr = res->ai_addr;

    if ((rawfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)
        err_sys("socket error");
    if (setsockopt(rawfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)
        err_sys("setsockopt error");

    bzero(buf, sizeof(buf));

	struct in_addr src;
	if (inet_pton(AF_INET, argv[1], &src) != 1)
		err_sys("inet_pton error");

	dlt_encap_ip((u_char *)buf, 0, src, ((struct sockaddr_in *)res->ai_addr)->sin_addr,
			htons((getpid() & 0xffff) | 0x8000), ((struct sockaddr_in *)servaddr)->sin_port,
			TH_SYN, 0, 0);

    if (sendto(rawfd, buf, 44, 0, res->ai_addr, res->ai_addrlen) < 0)
        err_sys("sendto error");

    dlt_tcp_pcap(res->ai_addr);

    freeaddrinfo(res);
    return 0;
}


void dlt_tcp_pcap(struct sockaddr *dest)
{
    char errbuf[PCAP_ERRBUF_SIZE];

    bzero(errbuf, sizeof(errbuf));
    assert(device != NULL);

    bzero(errbuf, sizeof(errbuf));
    if ((ppt = pcap_open_live(device, SNAPLEN, 1, TO_MS, errbuf)) == NULL) /* 1514 bytes, non-promiscuous mode, timeout = 1000 ms */ 
        err_quit("pcap_open_live error: %s", errbuf);
    if (strlen(errbuf) != 0)
        fprintf(stderr, "pcap_open_live warning: %s", errbuf);

    bpf_u_int32 localnet, netmask;
    bzero(errbuf, sizeof(errbuf));
    if (pcap_lookupnet(device, &localnet, &netmask, errbuf) < 0)
        err_quit("pcap_loopupnet error: %s", errbuf);

    char str1[64], str2[64];
    int srcport;
    char srchost[64];
    if (inet_ntop(AF_INET, &localnet, str1, sizeof(str1)) == NULL)
        err_sys("inet_ntop error");
    printf("localnet = %s, ", str1);
    if (inet_ntop(AF_INET, &netmask, str2, sizeof(str2)) == NULL)
        err_sys("inet_ntop error");
    printf("netmask = %s\n", str2);

    char filtercmd[MAXLINE];
    if (inet_ntop(AF_INET, &((struct sockaddr_in *)dest)->sin_addr, srchost, sizeof(srchost)) == NULL)
        err_sys("inet_ntop error");
    srcport = ntohs(((struct sockaddr_in *)dest)->sin_port);
    snprintf(filtercmd, sizeof(filtercmd),
            "tcp and src host %s and src port %d",
            srchost, srcport);
    printf("filtercmd = %s\n", filtercmd);

    struct bpf_program bpCode;
    if (pcap_compile(ppt, &bpCode, filtercmd, 0, netmask) < 0)
        err_quit("pcap_compile error: %s", pcap_geterr(ppt));

    if (pcap_setfilter(ppt, &bpCode) < 0)
        err_quit("pcap_setfilter eror: %s", pcap_geterr(ppt));

    if ((linktype = pcap_datalink(ppt)) < 0)
        err_quit("pcap_datalink error: %s", pcap_geterr(ppt));

    if (pcap_loop(ppt, 4, &dlt_handler, NULL) == -1)
        err_quit("pcap_loop error: %s", pcap_geterr(ppt));

    pcap_freecode(&bpCode);
    pcap_close(ppt);
}

void dlt_tcp_push(const u_char *packet)
{
    char buf[576], *ptr;
    int size_eth, userlen = 0;
    const struct ip *precv;
    const struct tcphdr *precvtcp;

	if (linktype == 0)
		size_eth = 4;
	else if (linktype == 1)
		size_eth = 14;
	else
		err_quit("unknown linktype: %d", linktype);

    precv = (struct ip *)(packet + size_eth);
	precvtcp = (struct tcphdr *)(packet + size_eth + precv->ip_hl * 4);

    struct sockaddr_in servaddr;
    servaddr.sin_addr = precv->ip_src;
    servaddr.sin_family = AF_INET;

	dlt_encap_ip((u_char *)buf, userlen, precv->ip_dst, precv->ip_src, precvtcp->th_dport, precvtcp->th_sport, 
			TH_ACK, htonl(ntohl(precvtcp->th_ack)), htonl(ntohl(precvtcp->th_seq) + 1));

    if (sendto(rawfd, buf, 44 + userlen, 0, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in)) < 0)
        err_sys("send error");

	ptr = buf + 44;
	strncpy(ptr, "just test\n", sizeof(buf) - 44);
	userlen = strlen(ptr);
#if (_DEBUG)
	printf("string = %s\n", ptr);
	printf("userlen = %d\n", userlen);
#endif
	dlt_encap_ip((u_char *)buf, userlen, precv->ip_dst, precv->ip_src, precvtcp->th_dport, precvtcp->th_sport, 
			(TH_ACK|TH_PUSH), htonl(ntohl(precvtcp->th_ack)), htonl(ntohl(precvtcp->th_seq) + 1));

    if (sendto(rawfd, buf, 44 + strlen(ptr), 0, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in)) < 0)
        err_sys("send error");

}

void dlt_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
	const struct ip *pip;
	const struct tcphdr *ptcp;
	const u_char *ptags;
	const u_char *str;
	int size_eth, size_ip, size_tcp;

	if (linktype == 0)
		size_eth = 4;
	else if (linktype == 1)
		size_eth = 14;
	else
		err_quit("unknown linktype: %d", linktype);

	pip = (struct ip *)(packet + size_eth);
	size_ip = pip->ip_hl * 4;
	ptcp = (struct tcphdr *)(packet + size_eth + size_ip);
	size_tcp = ptcp->th_off * 4;
	ptags = packet + size_eth + size_ip + 13;

	if ((*ptags) & 0x02) { /* SYN */
		printf("received SYN, will send PUSH\n");
		dlt_tcp_push(packet);
	} else if ((*ptags) & 0x08) { /* PUSH */
		printf("received PUSH, will send FIN\n");
		str = packet + size_eth + size_ip + size_tcp; 
		printf("capture data: ");
		fflush(stdout);
		write(1, str, header->caplen - size_eth - size_ip - size_tcp);
		puts("\n");
		fflush(stdout);
        dlt_tcp_fin(packet);
	} else if ((*ptags) & 0x01) { /* FIN */
		printf("received FIN, will send ACK\n");
        dlt_tcp_ack(packet);
	} else
		printf("unknown tags: %d\n", *ptags);
	return;
}

void dlt_encap_ip(u_char *buf, int userlen, struct in_addr src, struct in_addr dst, u_short sport, u_short dport, u_char flags, uint32_t seq, uint32_t ack)
{
	struct ip *pip;
	static int count = 0;
	bzero(buf, 44); /* hardcode, IP header + TCP header */

    pip = (struct ip *)buf;
    pip->ip_hl = 5; /* 5<<2 = 20 bytes */
    pip->ip_v = IPVERSION; /* IPv4 */
    pip->ip_tos = 0;
#if defined(linux) || defined(__OpenBSD__)
    pip->ip_len = htons(44 + userlen);
#else
    pip->ip_len = 44 + userlen;
#endif
    pip->ip_id = htons((u_short) (time(0) + count++) & 0xffff); /* id is 16 bits */
    pip->ip_off = IP_DF; /* Don't Fragment */
    pip->ip_ttl = 64;
    pip->ip_p = IPPROTO_TCP;
    pip->ip_src.s_addr = src.s_addr;
    pip->ip_dst.s_addr = dst.s_addr;
    pip->ip_sum = dlt_cksum((uint16_t *)pip, 20); /* IP checksum
                                                     only include
                                                     the header */
    /*
     * TCP header
     */
    struct tcphdr *ptcp;
    ptcp = (struct tcphdr *)(buf + 20);
    ptcp->th_sport = sport;
    ptcp->th_dport = dport;
    ptcp->th_seq = seq;
    ptcp->th_ack = ack;
    ptcp->th_off = 6; /* header length: (5 base + 1 opt) * 4 */
    ptcp->th_flags = flags;
    ptcp->th_urp = 0; /* urgent pointer */
    ptcp->th_win = htons(65535); /* default */
    ptcp->th_sum = 0; /* calculate later */

    /* MSS TCP option */
    u_char *ptr;
    ptr = buf + 40;
    *ptr++ = 2;
    *ptr++ = 4;
    *((u_short *)ptr) = htons(1460);


    /* now TCP checksum */
    struct pseudo_tcphdr pkt;
    pkt.ip_src = pip->ip_src;
    pkt.ip_dst = pip->ip_dst;
    pkt.pad = 0;
    pkt.ip_p = IPPROTO_TCP;
    pkt.tcp_len = htons(24 + userlen); /* must by network order */
    pkt.tcphdr = *ptcp;
    bzero(pkt.data, sizeof(pkt.data));
    memmove(pkt.data, buf + 40, 4 + userlen); /* 4 for TCP option length */
#if (_DEBUG)
	if (userlen != 0)
		printf("userdata = %s", buf + 44);
#endif
    ptcp->th_sum = dlt_cksum((uint16_t *)&pkt, 12 + 24 + userlen); /* pseudo tcphdr size (12) + 
																	  tcphdr size (24) + userdata size */
#if (_DEBUG)
    printf("id: %d\n", ntohs(pip->ip_id));
    printf("cksum: 0x%x\n", ntohs(ptcp->th_sum));
    printf("source port: %d\n", ntohs(ptcp->th_sport));
    printf("dest port: %d\n", ntohs(ptcp->th_dport));
	printf("seq: %u\n", ntohl(ptcp->th_seq));
	printf("ack: %u\n", ntohl(ptcp->th_ack));
#endif

	return;
}


void dlt_tcp_fin(const u_char *packet)
{
    u_char buf[576];
    int size_eth, userlen = 0;
    const struct ip *precv;
    const struct tcphdr *precvtcp;

	if (linktype == 0)
		size_eth = 4;
	else if (linktype == 1)
		size_eth = 14;
	else
		err_quit("unknown linktype: %d", linktype);

    precv = (struct ip *)(packet + size_eth);
	precvtcp = (struct tcphdr *)(packet + size_eth + precv->ip_hl * 4);

    struct sockaddr_in servaddr;
    servaddr.sin_addr = precv->ip_src;
    servaddr.sin_family = AF_INET;

	dlt_encap_ip(buf, userlen, precv->ip_dst, precv->ip_src, precvtcp->th_dport, precvtcp->th_sport, 
			(TH_ACK|TH_FIN), htonl(ntohl(precvtcp->th_ack)), htonl(ntohl(precvtcp->th_seq) + 1));

    if (sendto(rawfd, buf, 44 + userlen, 0, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in)) < 0)
        err_sys("send error");

}

void dlt_tcp_ack(const u_char *packet)
{
    u_char buf[576];
    int size_eth, userlen = 0;
    const struct ip *precv;
    const struct tcphdr *precvtcp;

	if (linktype == 0)
		size_eth = 4;
	else if (linktype == 1)
		size_eth = 14;
	else
		err_quit("unknown linktype: %d", linktype);

    precv = (struct ip *)(packet + size_eth);
	precvtcp = (struct tcphdr *)(packet + size_eth + precv->ip_hl * 4);

    struct sockaddr_in servaddr;
    servaddr.sin_addr = precv->ip_src;
    servaddr.sin_family = AF_INET;

	dlt_encap_ip(buf, userlen, precv->ip_dst, precv->ip_src, precvtcp->th_dport, precvtcp->th_sport, 
			TH_ACK, htonl(ntohl(precvtcp->th_ack)), htonl(ntohl(precvtcp->th_seq) + 1));

    if (sendto(rawfd, buf, 44 + userlen, 0, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in)) < 0)
        err_sys("send error");

}
