#include "raw_tracert.h"
#include <libgen.h>
#include <assert.h>

#define err_help(str) err_quit("Usage: %s [ -m max_ttl -w wait_time -p probe_times -n ] host",str)

struct raw_proto raw_proto = {NULL, NULL, NULL, NULL, 0};

int main(int argc, char *argv[])
{
    int c;
    struct addrinfo *paiRes, aiHints;

    nameconvert = 1;
    waittime = 3;
    datalen = sizeof(struct rawrec);
    max_ttl = 30;
    nprobes = 3;
    dport = 32768 + 666; /* initial dst port,
                            will be incremented by 1 */

    opterr = 0; /* getopt() not write to stderr */
    while ((c = getopt(argc, argv, "m:w:p:n")) != -1) {
        switch(c) {
            case 'm':
                if ((max_ttl = atoi(optarg)) <= 1)
                    err_help(basename(argv[0]));
                break;
            case 'w':
                waittime = atoi(optarg);
                break;
            case 'p':
                nprobes = atoi(optarg);
                break;
            case 'n': /* don't convert address to names */
                nameconvert--;
                break;
            case '?':
                err_help(basename(argv[0]));
        }
    }

    if (optind != argc-1)
        err_help(basename(argv[0]));
    host = argv[optind];
    aiHints.ai_flags = AI_CANONNAME;
    aiHints.ai_family = AF_INET;
    aiHints.ai_socktype = 0;
    aiHints.ai_protocol = 0;

    int errcode;
    if ((errcode = getaddrinfo(host, NULL, &aiHints, &paiRes)) != 0)
        err_quit("getaddrinfo error: %s", gai_strerror(errcode));

    assert(paiRes->ai_family == AF_INET);

    if (inet_ntop(AF_INET, &((struct sockaddr_in *)paiRes->ai_addr)->sin_addr, strHost, sizeof(strHost)) == NULL)
        err_sys("inet_ntop error");
    printf("rawtracert to %s (%s): %d hops max, %d data bytes\n",
            paiRes->ai_canonname ? paiRes->ai_canonname : strHost,
            strHost, max_ttl, datalen);

    pid = getpid();

    if (signal(SIGALRM, raw_sig_alrm) == SIG_ERR)
        err_sys("signal error");

    prp = &raw_proto;
    prp->psaSend = paiRes->ai_addr;
    prp->psaRecv = Calloc(1, paiRes->ai_addrlen);
    prp->psaLast = Calloc(1, paiRes->ai_addrlen);
    prp->psaBind = Calloc(1, paiRes->ai_addrlen);
    prp->slLen = paiRes->ai_addrlen;

    raw_traceloop();

    return 0;
}





