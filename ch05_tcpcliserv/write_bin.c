#include "unp.h"

int main(void)
{
    char buf[BUFSIZ];
    FILE *fp;

    if ((fp = fopen("./bin.txt", "w")) == NULL)
        err_sys("fopen error");
    while (fgets(buf, sizeof(buf), stdin) != NULL)
        if (!fwrite(buf, sizeof(buf), 1, fp))
            err_sys("fwrite error");
    if (ferror(stdin))
        err_sys("fgets error");

    return 0;
}

