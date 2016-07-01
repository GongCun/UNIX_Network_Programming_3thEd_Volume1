#include "mynames.h"

int main(int argc, char **argv)
{
    char *ptr;
    struct servent *sptr;

    while (--argc > 0) {
        ptr = *++argv;

        errno = 0;
        sptr = getservbyname(ptr, "tcp");
        if (sptr == NULL) {
            if (errno == 0)
                fprintf(stderr, "%s: port/tcp undefined\n", ptr);
            else
                err_sys("getservbyname error");
        }
        else
            printf("%s: %d/tcp\n", sptr->s_name, ntohs(sptr->s_port)); 

        errno = 0;
        sptr = getservbyname(ptr, "udp");
        if (sptr == NULL) {
            if (errno == 0)
                fprintf(stderr, "%s: port/udp undefined\n", ptr);
            else
                err_sys("getservbyname error");
        }
        else
            printf("%s: %d/udp\n", sptr->s_name, ntohs(sptr->s_port)); 
    }
    
    exit(0);
}

        
