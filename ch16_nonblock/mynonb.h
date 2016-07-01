#ifndef _MY_NONB_H
#define _MY_NONB_H

#include "unp.h"
#include <time.h>

void strclinonb(FILE *, int);
char *gf_time(void);
int connect_nonb(int, const SA *, socklen_t, int);

#endif
