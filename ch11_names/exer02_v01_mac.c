#include "mynames.h"

int main(int argc, char **argv)
{
    char *ptr, **pptr;
    char str[16]; /* ddd.ddd.ddd.ddd\0 */
    struct hostent *hptr;
    struct in_addr addrlist[35]; /* gethostbyname can return up to 
                                    35 alias/address pointers (UNP p418) */
    int i, n;

    while (--argc > 0) {
        ptr = *++argv;
        if ((hptr = gethostbyname(ptr)) == NULL) {
            err_msg("gethostbyname error for host: %s: %s",
                    ptr, hstrerror(h_errno));
            continue;
        }
        printf("official hostname: %s\n", hptr->h_name);

        for (pptr = hptr->h_aliases; *pptr != NULL; pptr++)
            printf("\talias: %s\n", *pptr);

        switch (hptr->h_addrtype) {
            case AF_INET:
                i = 0;
                pptr = hptr->h_addr_list;
                for (; *pptr != NULL; pptr++) 
                    memcpy(&addrlist[i++], *pptr, sizeof(struct in_addr));
                n = i;

                for (i = 0; i < n; i++) {
                    printf("\taddress: %s\n",
                            Inet_ntop(hptr->h_addrtype, &addrlist[i], str, sizeof(str)));
                    hptr = gethostbyaddr(&addrlist[i], hptr->h_length, hptr->h_addrtype);
                    if (hptr == NULL)
                        err_msg("gethostbyaddr error: %s", hstrerror(h_errno));
                    else if (hptr ->h_name != NULL)
                        printf("\tname: %s\n", hptr->h_name);
                    else
                        printf("\t(no hostname returned by gethostbyaddr)\n");
                }
                break;
            default:
                err_ret("unknown address type");
                break;
        }
    }
    exit(0);

}
