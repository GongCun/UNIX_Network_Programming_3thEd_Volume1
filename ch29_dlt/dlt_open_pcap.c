#include "udpcksum-ex.h"
pcap_t *ppt; /* pointer to struct pcap_t */
int snaplen = 1518;
int to_ms = 1000;
int linktype;

/* open and initializes packet capture device. */
void dlt_open_pcap(void)
{
    char errbuf[PCAP_ERRBUF_SIZE];

    bzero(errbuf, sizeof(errbuf));

    /* If the capture device was not specified, let pcap_lookupdev
     * choose device. It issue the SIOCGIFCONF ioctl and choose the
     * lowest numbered device that is up.
     * */
    if (device == NULL) {
        if ((device = pcap_lookupdev(errbuf)) == NULL)
            err_quit("pcap_lookupdev error: %s", errbuf);
    }
    printf("device = %s\n", device);

    bzero(errbuf, sizeof(errbuf));
    if ((ppt = pcap_open_live(device, snaplen, 0, to_ms, errbuf)) == NULL) /* 200 bytes, non-promiscuous mode, timeout = 500 ms */ 
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
            "(icmp and src host %s) or (udp and src host %s and src port %d)",
            srchost, srchost, srcport);
    printf("filtercmd = %s\n", filtercmd);

    struct bpf_program bpCode;
    if (pcap_compile(ppt, &bpCode, filtercmd, 0, netmask) < 0)
        err_quit("pcap_compile error: %s", pcap_geterr(ppt));

    if (pcap_setfilter(ppt, &bpCode) < 0)
        err_quit("pcap_setfilter eror: %s", pcap_geterr(ppt));

    /*  pcap_datalink returns the type of datalink for the packet capture
     *  device. We need this when receiving packets to determine the size of
     *  the datalink header that will be at the beginning of each packet we
     *  read.
     *  */
    if ((linktype = pcap_datalink(ppt)) < 0 || linktype == PCAP_ERROR_NOT_ACTIVATED)
        err_quit("pcap_datalink error: %s", pcap_geterr(ppt));
    printf("link type = %d\n", linktype);
}





