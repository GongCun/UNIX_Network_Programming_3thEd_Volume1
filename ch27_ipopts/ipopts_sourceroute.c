#include "unp.h"
#include <netinet/in_systm.h>
#include <netinet/ip.h>

static u_char *pchOpt; /* pointer into options being formed */
static u_char *pchLen; /* pointer to length by in SRR option */
static int iOcnt; /* count of # addressed */

u_char *ipopts_inet_srcrt_init(int type)
    /* return the pointer for setsockopt() */
{
    pchOpt = Malloc(44); /* NOP, code, len, ptr, up to 10 addressed */
    bzero(pchOpt, 44); /* guarantees EOLs at end */
    
    iOcnt = 0;
    *pchOpt++ =  IPOPT_NOP; /* defined in <netinet/ip.h> */
    *pchOpt++ = type ? IPOPT_SSRR : IPOPT_LSRR; /* <netinet/ip.h> */
    pchLen = pchOpt++;
    *pchOpt++ = 4; /* offset to first address */

    return pchOpt - 4;
}

int ipopts_inet_srcrt_add(char *pchHoststr)
    /* return size for setsockopt() */
{
    struct in_addr iaHostaddr;
    int n, len;

    if (iOcnt > 9)
        err_quit("too many source routes");

    if ((n = inet_pton(AF_INET, pchHoststr, &iaHostaddr)) < 0)
        err_sys("inet_pton error for %s", pchHoststr);
    else if (n == 0)
        err_quit("inet_pton error for %s", pchHoststr);
    memmove(pchOpt, &iaHostaddr, sizeof(struct in_addr));

    pchOpt += sizeof(struct in_addr);
    iOcnt++;
    len = 3 + iOcnt * sizeof(struct in_addr);
    *pchLen = len;
    return len + 1;
}

void ipopts_inet_srcrt_print(u_char *ptr, int len)
{
    u_char c;
    char hoststr[64];

#ifdef _BSD
    struct in_addr hostaddr;
    memmove(&hostaddr, ptr, sizeof(struct in_addr));
    ptr += sizeof(struct in_addr);
#endif

    while ((c = *ptr++) == IPOPT_NOP) /* skip any leading NOPs */
        ;

    if (c == IPOPT_SSRR)
        printf("received SSRR: ");
    else if (c == IPOPT_LSRR)
        printf("received LSRR: ");
    else {
        printf("received option type %d\n", c);
            return;
    }
#ifdef _BSD
    printf("%s ", Inet_ntop(AF_INET, &hostaddr, hoststr, sizeof(hoststr)));
    len = *ptr++ - sizeof(struct in_addr); /* substract dest IP addr */
#else
    len = *ptr++ - 4; /* Linux or Solaris not set the dest IP addr
                         in the head, but need substract 4 bytes */
#endif

    ptr++; /* skip over pointer */
    while (len > 0) {
        printf("%s ", Inet_ntop(AF_INET, ptr, hoststr, sizeof(hoststr)));
        ptr += sizeof(struct in_addr);
        len -= sizeof(struct in_addr);
    }
    printf("\n");
}



