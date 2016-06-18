#include "unp.h"
#include <pcap.h>
#ifdef AIX
  #include <net/bpf.h>
  #include <netinet/if_ether.h>
#else
  #include <net/ethernet.h>
#endif

#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <libgen.h>
#include <time.h>

#define SNAPLEN 1518
#define PAKNUM -1 /* infinity */ 
void handler(u_char *, const struct pcap_pkthdr *, const u_char *);
void handler_udp(const struct pcap_pkthdr *header, const u_char *packet);
void dlt_print_payload(const u_char *, int);

static int linktype;
static int fd;

int main(int argc, char *argv[]) /* dlt_sniff dev expression */
{
    char *strdev, filterexp[MAXLINE];
    if (argc != 3)
        err_quit("Usage: %s dev 'filter_exp'", basename(argv[0]));

    strdev = argv[1];
    strncpy(filterexp, argv[2], sizeof(filterexp));
#ifdef _DEBUG
    printf("filterexp = %s\n", filterexp);
#endif

	/* open file to record the payload */
	if ((fd = open("./record.pcap", O_RDWR|O_CREAT|O_TRUNC, 0644)) < 0)
		err_sys("open file error");

    bpf_u_int32 mask;
    bpf_u_int32 net;
    char errbuf[PCAP_ERRBUF_SIZE];

    if(pcap_lookupnet(strdev, &net, &mask, errbuf) < 0) {
        fprintf(stderr, "Can't get netmask for device %s: %s\n", strdev, errbuf);
        mask = 0;
        net = 0;
    }

    char str[64];
    if (inet_ntop(AF_INET, &net, str, sizeof(str)) == NULL)
        err_sys("inet_ntop error");
    printf("%s net: %s\n", strdev, str);
    if (inet_ntop(AF_INET, &mask, str, sizeof(str)) == NULL)
        err_sys("inet_ntop error");
    printf("%s mask: %s\n", strdev, str);

    pcap_t *ppt;
    bzero(errbuf, sizeof(errbuf));
    if ((ppt = pcap_open_live(strdev, SNAPLEN, 1, 1000, errbuf)) == NULL)
        err_quit("pcap_open_live error: %s", errbuf);
    if (strlen(errbuf) != 0)
        fprintf(stderr, "Warning: %s", errbuf);

    if ((linktype = pcap_datalink(ppt)) < 0)
        err_quit("pcap_datalink error: %s", pcap_geterr(ppt));
    if (linktype == 0) /* DLT_NULL: 0 */
		printf("loopback\n");
	else if (linktype == 1) /* DLT_EN10MB: 1 */
		printf("ethernet\n");
	else
        err_quit("unsupported datalink (%d)", linktype);

    struct bpf_program bpCode;
    if (pcap_compile(ppt, &bpCode, filterexp, 0, mask) < 0)
        err_quit("pcap_compile error: %s", pcap_geterr(ppt));
    if (pcap_setfilter(ppt, &bpCode) < 0)
        err_quit("pcap_setfilter error: %s", pcap_geterr(ppt));
    
    /* now setup callback function */
    if (pcap_loop(ppt, PAKNUM, &handler, NULL) == -1)
        err_quit("pcap_loop error: %s", pcap_geterr(ppt));

    /* cleanup */
    pcap_freecode(&bpCode);
    pcap_close(ppt);
	close(fd);

    printf("\nCapture complete.\n");

    return 0;
}


void handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    static int count = 0;
    printf("\nPacket number %d:\n", count++);

	/* get daytime */
	char tmbuf[64], buf[64];
	struct tm *tm = localtime(&(header->ts.tv_sec));
	strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", tm);
	snprintf(buf, sizeof(buf), "%s.%06d", tmbuf, header->ts.tv_usec);
	printf("%s\n", buf);

    /* const struct ether_header *peth; */
    const struct ip *pip;
    const struct tcphdr *ptcp;
    const u_char *payload;

    /* peth = (struct ether_header *)packet; */
    int size_eth = 14;
    if (linktype == 0)
        size_eth = 4;
    int size_ip;
    pip = (struct ip *)(packet + size_eth);
    size_ip = pip->ip_hl * 4;
    if (size_ip < 20) {
        fprintf(stderr, "   * Invalid IP header length: %u bytes\n", size_ip);
		return;
	}

    char strsrc[64], strdst[64];
    if (inet_ntop(AF_INET, &pip->ip_src, strsrc, sizeof(strsrc)) == NULL)
        err_sys("inet_ntop error");
    if (inet_ntop(AF_INET, &pip->ip_dst, strdst, sizeof(strdst)) == NULL)
        err_sys("inet_ntop error");
    printf("From %s to %s\n", strsrc, strdst);

    if (pip->ip_p != IPPROTO_TCP && pip->ip_p != IPPROTO_UDP) {
        fprintf(stderr, "unsupported protocol: %d\n", pip->ip_p);
        return;
    }
    if (pip->ip_p == IPPROTO_UDP) {
        handler_udp(header, packet);
        return;
    }

	/* tcp */
    ptcp = (struct tcphdr *)(packet + size_eth + size_ip);
	int size_tcp = ptcp -> th_off * 4;
	if (size_tcp < 20) {
        fprintf(stderr, "   * Invalid TCP header length: %u bytes\n", size_tcp);
		return;
	}
	printf("Src port: %d, Dst port %d\n", ntohs(ptcp->th_sport), ntohs(ptcp->th_dport));
	printf("Seq: %u, Ack %u\n", ntohl(ptcp->th_seq), ntohl(ptcp->th_ack));

	payload = (u_char *)(packet + size_eth + size_ip + size_tcp);
	int size_payload;
#ifdef _DEBUG
	printf("%d\n", header->caplen - size_eth - size_ip -size_tcp);
	printf("%d\n", ntohs(pip->ip_len) - size_ip - size_tcp);
#endif
	size_payload = header->caplen - size_eth - size_ip -size_tcp;

	if (size_payload > 0) {
		printf("Payload (%d bytes).\n", size_payload);
		dlt_print_payload(payload, size_payload);
	}

    return;
}

void dlt_print_payload(const u_char *payload, int len)
{
    if (write(fd, payload, len) != len)
        perror("write error");
}

void handler_udp(const struct pcap_pkthdr *header, const u_char *packet)
{
    const struct ip *pip;
    const struct udphdr *pudp;
    u_char *payload;
    int size_eth, size_ip, size_udp = sizeof(struct udphdr), size_payload;

    size_eth = (linktype == 0) ? 4 : 14;
    pip = (struct ip *)(packet + size_eth);
    if ((size_ip = pip->ip_hl * 4) < 20) {
        fprintf(stderr, "   * Invalid IP header length: %u bytes\n", size_ip);
		return;
    }

    pudp = (struct udphdr *)(packet + size_eth + size_ip);
    if (header->caplen - size_eth - size_ip < size_udp) {
        fprintf(stderr, "   * Invalid UDP header length: %u bytes\n", header->caplen - size_eth - size_ip);
        return;
    }
    printf("Src port: %d, Dst port %d\n", ntohs(pudp->uh_sport), ntohs(pudp->uh_dport));
    printf("data length = %d\n", ntohs(pudp->uh_ulen));
    printf("check sum = 0x%x\n", pudp->uh_sum);

    payload = (u_char *)(packet + size_eth + size_ip + size_udp);
    size_payload = header->caplen - size_eth - size_ip - size_udp;
    if (size_payload > 0) {
        printf("payload (%d bytes).\n", size_payload);
        dlt_print_payload(payload, size_payload);
    }
    return;
}

