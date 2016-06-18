#include "raw_ping.h"
#include <assert.h>
#include <libgen.h>

struct raw_proto raw_proto = {raw_proc, raw_send, NULL, NULL, NULL, 0, IPPROTO_ICMP};

int datalen = 56; /* # bytes of data following ICMP header
                     datalen = icmplen - 8 */
int nsent = 0;

int main(int argc, char *argv[])
{
    struct addrinfo *paiRes, aiHints;
    
    host = argv[1];

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
    prp->sasend = paiRes->ai_addr;
    if ((prp->sarecv = calloc(1, paiRes->ai_addrlen)) == NULL)
        err_sys("calloc error");
    prp->salen = paiRes->ai_addrlen;

    raw_readloop();
    return 0;
}

