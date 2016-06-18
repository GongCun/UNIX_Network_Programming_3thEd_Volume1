#include "unp.h"
#include <libgen.h>
#include <assert.h>

extern u_char *ipopts_inet_srcrt_init(int);
extern int ipopts_inet_srcrt_add(char *);
extern void ipopts_inet_srcrt_print(u_char *, int);
extern void ipopts_str_cli(FILE *, int);

static void err_help(char *str)
{
    err_quit("Usage: %s [-ls] IPaddress1 ... DSTaddress #Port", str);
}


int main(int argc, char *argv[])
{
    char ch;
    u_char *ptr = NULL;

    opterr = 0; /* don't want getopt() writing to stderr */
    optind = 1;

    while ((ch = getopt(argc, argv, "ls")) != -1) {
        switch(ch) {
            case 'l': /* LSRR: loose source route */
                if (ptr)
                    err_quit("can't use both -l and -s");
                ptr = ipopts_inet_srcrt_init(0);
                break;
            case 's': /* SSRR: strict source route */
                if (ptr)
                    err_quit("can't use both -l and -s");
                ptr = ipopts_inet_srcrt_init(1);
                break;
            case '?':
                err_help(basename(argv[0]));
        }
    }
    int len = 0;
    if (ptr) {
        while (optind < argc - 2)
            len = ipopts_inet_srcrt_add(argv[optind++]);
    } else if (optind != argc - 2)
        err_help(basename(argv[0]));
    if (optind != argc - 2)
        err_help(basename(argv[0]));

    struct sockaddr_in servaddr;
    int n;

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    assert(optind == argc - 2);
    if ((n = inet_pton(AF_INET, argv[optind++], &servaddr.sin_addr)) < 0)
        err_sys("inet_pton error");
    else if (n == 0)
        err_quit("inet_pton error");
    servaddr.sin_port = htons(atoi(argv[optind]));

    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        err_sys("socket error");

    if (ptr) {
        len = ipopts_inet_srcrt_add(argv[argc-2]); /* dest at end */
        assert(len > 0);

        if (setsockopt(sockfd, IPPROTO_IP, IP_OPTIONS, ptr, len) < 0)
            err_sys("setsockopt error");
        free(ptr);
    }
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
        err_sys("connect error");
#ifdef _CLIENT_CLEAN_SSR
    if (setsockopt(sockfd, IPPROTO_IP, IP_OPTIONS, NULL, 0) < 0)
        err_sys("setsockopt clean IP_OPTIONS error");
#endif
    ipopts_str_cli(stdin, sockfd);
    return 0;
}

