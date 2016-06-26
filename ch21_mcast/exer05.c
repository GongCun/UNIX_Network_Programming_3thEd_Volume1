#include "unp.h"

int main(void)
{
    uint32_t useci;
    double usecf;

    useci = 4294967295; /* 2**32 - 1 */
    usecf = useci; /* integer fraction -> double */
    usecf /= 4294967296.0; /* divide by 2**32 -> [0, 1) */
    printf("usecf = %.20f\n", usecf);
    useci = usecf * 1000000.0; /* fraction -> parts per milion */
    printf("useci = %d\n", useci);

    exit(0);
}

