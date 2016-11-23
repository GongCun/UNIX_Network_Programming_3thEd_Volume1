#include "raw_ping.h"
#include <assert.h>
#include <libgen.h>

struct raw_proto raw_proto = {raw_proc, raw_send, NULL, NULL, NULL, 0, IPPROTO_ICMP};

int datalen = 56; /* # bytes of data following ICMP header
                     datalen = icmplen - 8 */
int nsent = 0;
struct in_addr src;
int src_flag = 0;
int raw_hdr = 0;
int xmtu;
int verbose;
int offunit;
int offsize;
char dev[IFNAMSIZ];

static void usage(void)
{
	err_quit("Usage: rawping -s <packet_size> -S <source_ip> -M <mtu> host");
}



int main(int argc, char *argv[])
{
    struct addrinfo *paiRes, aiHints;
    int c;


    opterr = 0;
    optind = 1;
    while ((c = getopt(argc, argv, "vs:S:M:")) != -1)
            switch (c) {
                    case 'v':
                            verbose = 1;
                            break;
                    case 's':
                            if (optarg) datalen = atoi(optarg);
			    if (datalen >= BUFSIZE - 20 - 8)
				    err_quit("too large message");
                            break;
                    case 'S':
                            if (optarg && inet_aton(optarg, &src) != 1)
                                    err_quit("inet_aton() error");
                            src_flag = 1;
                            break;
                    case 'M':
                            if (optarg) xmtu  = atoi(optarg);
                            raw_hdr = 1;
                            break;
                    case '?':
			    usage();
            }


    if (optind != argc - 1)
	    usage();

    if (raw_hdr && !src_flag)
            err_quit("Must use -S options");


    host = argv[optind];

    aiHints.ai_flags = AI_CANONNAME;
    aiHints.ai_family = AF_INET;
    aiHints.ai_socktype = 0;
    aiHints.ai_protocol = 0;

    pid = getpid() & 0xffff; /* ICMP ID field is 2 bytes */
    if (signal(SIGALRM, raw_sig_alrm) == SIG_ERR)
        err_sys("signal error");

    int errcode;
    if ((errcode = getaddrinfo(host, NULL, &aiHints, &paiRes)) != 0)
        err_quit("getaddrinfo error: %s", gai_strerror(errno));

    assert(paiRes->ai_family == AF_INET);

    if (inet_ntop(AF_INET, &(((struct sockaddr_in *)paiRes->ai_addr)->sin_addr), strHost, sizeof(strHost)) == NULL)
        err_sys("inet_ntop error");
    printf("%s %s (%s): %d data bytes\n", basename(argv[0]),
            paiRes->ai_canonname ? paiRes->ai_canonname : strHost, strHost, datalen);

    prp = &raw_proto; /* IPv4 */
    if (raw_hdr)
	    prp->fsend = raw_send_hdr;
    prp->sasend = paiRes->ai_addr;
    if ((prp->sarecv = calloc(1, paiRes->ai_addrlen)) == NULL)
        err_sys("calloc error");
    prp->salen = paiRes->ai_addrlen;

    if (xmtu) {
	    offunit = (xmtu - 20) / 8;
	    offsize = offunit * 8;
	    if (verbose)
		    printf("Set MTU = %d, offset = %d\n", xmtu, offsize);
    }
    raw_readloop();
    return 0;
}

