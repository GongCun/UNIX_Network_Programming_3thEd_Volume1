#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char buf1[BUFSIZ];
    char *buf2;

    buf2 = alloca(BUFSIZ);

    printf("sizeof buf1 = %zd\n", sizeof(buf1));
    printf("sizeof buf2 = %zd\n", sizeof(buf2));
    exit(0);
}
