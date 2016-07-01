#include "unp.h"

int main(void)
{
    char buf[BUFSIZ];
    FILE *fp;

    if ((fp = fopen("./bin.txt", "r")) == NULL)
        err_sys("fopen error");
    while (fread(buf, sizeof(buf), 1, fp))
        if (fputs(buf, stdout) == EOF)
            err_sys("fputs error");
    if (ferror(fp))
        err_sys("fread error");

    return 0;
}

