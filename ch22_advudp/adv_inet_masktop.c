#include "unp.h"

/*
struct sockaddr {
    uint8_t sa_len;         # 1 byte 
    sa_family_t sa_family;  # 1 byte 
    char sa_data[14];       # addr starts from sa_data[2], offset=4bytes 
};
So length of mask address is: {0, 5, 6, 7, 8} bytes
*/

const char *
adv_inet_masktop(SA *sa, socklen_t salen)
{
    static char str[46];
    unsigned char *ptr = &sa->sa_data[2];

#if ! defined LINUX
    switch (sa->sa_len) {
        case(0): return ("0.0.0.0");
        case(5): snprintf(str, sizeof(str), "%d.0.0.0", *ptr); break;
        case(6): snprintf(str, sizeof(str), "%d.%d.0.0", *ptr, *(ptr+1)); break;
        case(7): snprintf(str, sizeof(str), "%d.%d.%d.0",
                         *ptr, *(ptr+1), *(ptr+2)); break;
        case(8): snprintf(str, sizeof(str), "%d.%d.%d.%d",
                         *ptr, *(ptr+1), *(ptr+2), *(ptr+4)); break;
        default: snprintf(str, sizeof(str), "(Unknown mask, len = %d, family = %d)",
                         sa->sa_len, sa->sa_family); break;
    }
#else
    switch(strlen(ptr)) { /* Linux not defined sa->sa_len */
        case(0): return ("0.0.0.0"); break;
        case(1): snprintf(str, sizeof(str), "%d.0.0.0", *ptr); break;
        case(2): snprintf(str, sizeof(str), "%d.%d.0.0", *ptr, *(ptr+1)); break;
        case(3): snprintf(str, sizeof(str), "%d.%d.%d.0",
                         *ptr, *(ptr+1), *(ptr+2)); break;
        case(4): snprintf(str, sizeof(str), "%d.%d.%d.%d",
                         *ptr, *(ptr+1), *(ptr+2), *(ptr+4)); break;
        default: snprintf(str, sizeof(str), "(Unknown mask, family = %hu)", sa->sa_family); break;
                 /* __SOCKADDR_COMMON (sa_)=sa_family (unsigned short int) */
    }
#endif

    return (str);

}
