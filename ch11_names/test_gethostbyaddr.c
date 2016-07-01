#include "mynames.h"

int main(int argc, char **argv)
{
    char *ptr;
    struct in_addr addr;
    struct hostent *hptr;
    
    while (--argc > 0) {
        ptr = *++argv;
        if (inet_aton(ptr, &addr) == 0) {
            err_msg("inet_aton error: %s", ptr);
            continue;
        }

        hptr = gethostbyaddr(&addr, sizeof(struct in_addr), AF_INET);
        if (hptr == NULL) {
            err_msg("gethostbyarr error for host: %s: %s", ptr, hstrerror(h_errno));
            continue;
        }
        printf("official hostname: %s\n", hptr->h_name);
    }

    exit(0);

}

