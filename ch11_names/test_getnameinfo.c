#include "mynames.h"

int main(int argc, char **argv)
{
    char host[MAXLINE], serv[32];
    char *ptr;
    struct sockaddr_in *saptr;
    int ret;

    saptr = malloc(sizeof(struct sockaddr_in));
    if (saptr == NULL)
        err_sys("malloc error");
    
    while (--argc > 0) {
        ptr = *++argv;
        bzero(saptr, sizeof(struct sockaddr_in));
        saptr->sin_family = AF_INET;
        if (inet_aton(ptr, &saptr->sin_addr) == 0) {
            err_msg("inet_aton error: %s", ptr);
            continue;
        }

        ret = getnameinfo((SA *)saptr, sizeof(struct sockaddr_in),
                host, sizeof(host),
                serv, 0, NI_NAMEREQD|NI_NOFQDN);
        if (ret != 0) {
            err_msg("getnameinfo error for host: %s: %s", ptr, gai_strerror(ret));
            continue;
        }
        printf("official hostname: %s\n", host);
    }

    free(saptr);
    exit(0);

}

