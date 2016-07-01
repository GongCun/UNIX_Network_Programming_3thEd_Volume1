#include	"myadvio.h"

void
exer04_str_echo(int sockfd)
{
	char		buf[MAXLINE];
    FILE *fpin, *fpout;

    if ((fpin = fdopen(sockfd, "r")) == NULL)
        err_sys("fdopen error");
    if ((fpout = fdopen(sockfd, "w")) == NULL)
        err_sys("fdopen error");

again:
	while ( fgets(buf, MAXLINE, fpin) != NULL) {
        if (fputs(buf, fpout) == EOF)
            err_sys("fputs error");
        /* fflush(fpout); */
    }

	if (ferror(fpin)) {
       if (errno == EINTR)
           goto again;
       err_sys("str_echo: read error");
    }
}
