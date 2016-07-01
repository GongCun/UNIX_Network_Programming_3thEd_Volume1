#include "mynames.h"

int main(int argc, char **argv)
{
    int port;
    struct servent *sptr;

    while (--argc > 0) {
        port = atoi(*++argv);

        errno = 0;
        sptr = getservbyport(htons(port), "tcp");
        if (sptr == NULL) {
            if (errno == 0)
                fprintf(stderr, "service: %d/tcp undefined\n", port);
            else
                err_sys("getservbyport error");
        }
        else
            printf("%s: %d/tcp\n", sptr->s_name, port);

        errno = 0;
        sptr = getservbyport(htons(port), "udp");
        if (sptr == NULL) {
            if (errno == 0)
                fprintf(stderr, "service: %d/udp undefined\n", port);
            else
                err_sys("getservbyport error");
        }
        else
            printf("%s: %d/udp\n", sptr->s_name, port);
    }
    
    exit(0);
}

        
