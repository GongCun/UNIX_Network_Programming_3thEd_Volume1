#include "myweb.h"

void home_page(const char *host, const char *fname)
{
    int fd, n;
    char line[MAXLINE];

    fd = Tcp_connect(host, SERV); /* blocking connect() */
    n = snprintf(line, MAXLINE, GET_CMD, fname);
    Writen(fd, line, n);

    for (;;) {
        if ((n = Read(fd, line, MAXLINE)) == 0)
            break;
        printf("read %d bytes from home page\n", n);

        /* do whatever with data, such as parsing the HTML */

    }
    printf("end-of-file on home page\n");
    Close(fd);
}
