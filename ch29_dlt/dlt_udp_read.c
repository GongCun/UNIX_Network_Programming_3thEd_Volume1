#include "udpcksum-ex.h"

const u_char *dlt_next_pcap(int *pLen);

struct udpiphdr *dlt_icmp_read(const u_char *ptr, int len);

struct udpiphdr *dlt_udp_read(void)
{
    const u_char *ptr;
    int len, size_eth, size_ip;
    const struct ip *pip;
    const struct udphdr *pudp;
    struct udpiphdr *pui;

    for (;;) {
        ptr = dlt_next_pcap(&len);
        switch (linktype) {
            case 0:
                size_eth = 4;
                break;
            case 1:
                size_eth = 14; /* 6src + 6dst + 2(length of type) */
                break;
            default:
                err_quit("can't handle the linktype: %d", linktype);
        }
        pip = (struct ip *)(ptr + size_eth);

        if ((size_ip = pip->ip_hl * 4) < 20) {
            fprintf(stderr, "* Invalid IP header length: %u bytes\n", size_ip);
            continue;
        }
        if (pip->ip_p == IPPROTO_ICMP) {
            printf("capture ICMP packet\n");
            if ((pui = dlt_icmp_read(ptr + size_eth + size_ip, len - size_eth - size_ip)) != NULL)
                return pui;
            else
                continue;
        }

        if (pip->ip_p != IPPROTO_UDP) {
            fprintf(stderr, "* not UDP packet: protocol = %d\n", pip->ip_p);
            continue;
        }
        if (dlt_cksum((uint16_t *)pip, size_ip) != 0) { /* IP checksum on the header length */
            fprintf(stderr, "* IP checksum error\n");
            continue;
        }
        pudp = (struct udphdr *)(ptr + size_eth + size_ip);
        if (len - size_eth - size_ip < sizeof(struct udphdr)) {
            fprintf(stderr, "* Invalid UDP header length: %u bytes\n",
                    len - size_eth - size_ip);
            continue;
        }
#if (_DEBUG)
        printf("capture UDP cksum = 0x%x\n", ntohs(pudp->uh_sum));
        u_char *payload = (u_char *)(ptr + size_eth + size_ip + sizeof(struct udphdr));
        int size_payload = len - size_eth - size_ip - sizeof(struct udphdr);
        struct pseudo_udphdr pseudo_udphdr;
        pseudo_udphdr.ip_src = pip->ip_src;
        pseudo_udphdr.ip_dst = pip->ip_dst;
        pseudo_udphdr.pad = 0;
        pseudo_udphdr.ip_p = IPPROTO_UDP;
        pseudo_udphdr.uh_ulen = pudp->uh_ulen;
        pseudo_udphdr.udphdr = *pudp;
        memmove(pseudo_udphdr.data, payload, size_payload);

        printf(">> calculate UDP checksum = 0x%x\n",
                dlt_cksum((uint16_t *)&pseudo_udphdr,
                    sizeof(struct in_addr) + /* 4 bytes */
                    sizeof(struct in_addr) + /* 4 bytes */
                    sizeof(u_char) + sizeof(u_char) + /* 2 bytes */
                    sizeof(u_short) + /* 2 bytes */
                    sizeof(struct udphdr) + /* 8 bytes */
                    size_payload));

        char strsrc[64], strdst[64];
        /* Don't use inet_ntoa(), it will share the same pointer */
        if (inet_ntop(AF_INET, &pip->ip_src, strsrc, sizeof(strsrc)) == NULL)
            err_sys("inet_ntop error");
        if (inet_ntop(AF_INET, &pip->ip_dst, strdst, sizeof(strdst)) == NULL)
            err_sys("inet_ntop error");
        printf("capture from %s:%d to %s:%d\n",
                strsrc, ntohs(pudp->uh_sport),
                strdst, ntohs(pudp->uh_dport));
#endif
        return (struct udpiphdr *)(ptr + size_eth);
    }
}

const u_char *dlt_next_pcap(int *pLen)
{
    const u_char *ptr;
    struct pcap_pkthdr *pPkth;


    if ((ptr = malloc(1518)) == NULL)
        err_sys("malloc error");
    while (pcap_next_ex(ppt, &pPkth, &ptr) != 1)
        ;
    *pLen = pPkth->caplen;

#if (_DEBUG)
    printf("capture protocol: %d\n", ((struct ip *)(ptr+14))->ip_p);
    printf("capture total len: %d\n", pPkth->caplen);
#endif
    return ptr;

}
struct udpiphdr *dlt_icmp_read(const u_char *ptr, int len)
{
    const struct icmp *picmp;
    const struct ip *pip;
    const struct udphdr *pudp;
    picmp = (struct icmp *)ptr;
    printf("Capture ICMP checksum = 0x%x\n", ntohs(picmp->icmp_cksum));
    printf("calculate ICMP checksum = 0x%x\n", dlt_cksum((uint16_t *)picmp, len));
    if (picmp->icmp_type == ICMP_UNREACH && picmp->icmp_code == ICMP_UNREACH_PORT) {
        fprintf(stderr, "ICMP port unreachable\n");
        if (len < 8 + sizeof(struct ip)) {
            fprintf(stderr, "short of data\n");
            return NULL;
        }
        pip = (struct ip *)(ptr + 8);
        if (pip->ip_hl * 4 < 20) {
            fprintf(stderr, "short of data\n");
            return NULL;
        }
        /*
         * We can fetch the original IP header data.
         */
        char strsrc[64], strdst[64];
        if (inet_ntop(AF_INET, &pip->ip_src, strsrc, 64) == NULL)
            err_sys("inet_ntop error");
        if (inet_ntop(AF_INET, &pip->ip_dst, strdst, 64) == NULL)
            err_sys("inet_ntop error");
        fprintf(stderr, "Original from %s to %s, IP checksum = 0x%x\n",
                strsrc, strdst, ntohs(pip->ip_sum));
        if (ntohs(pip->ip_sum)) {
            fprintf(stderr, "calculate original IP checksum = 0x%x\n",
                    dlt_cksum((uint16_t *)pip, pip->ip_hl*4)); /* the IP gen itself */
        }

        if (len - 8 - pip->ip_hl * 4 < 8) {
            fprintf(stderr, "short of data\n");
            return NULL;
        }
        pudp = (struct udphdr *)(ptr + 8 + pip->ip_hl * 4);
        fprintf(stderr, "capture UDP checksum in ICMP: 0x%x\n", ntohs(pudp->uh_sum)); /* as the same as sent checksum */
        fprintf(stderr, "Original from %s:%d to %s:%d\n", strsrc, ntohs(pudp->uh_sport), strdst, ntohs(pudp->uh_dport));
        return (struct udpiphdr*)(ptr + 8);
    } else
        return NULL;
}

