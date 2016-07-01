#ifndef _MY_ADVIO_H
#define _MY_ADVIO_H

#include "unp.h"
int my_connect_timeo(int, const SA *, socklen_t, long, long);
void my_dg_cli(FILE *, int, const SA *, socklen_t, long, long);
void my_dg_cli2(FILE *, int, const SA *, socklen_t, long, long);
void my_dg_cli3(FILE *, int, const SA *, socklen_t, long, long);
int my_readable_timeo(int, long, long);
void my_str_echo(int);

int exer01_connect_timeo(int, const SA *, socklen_t, long, long);
int exer02_connect_timeo(int, const SA *, socklen_t, long, long);
void exer04_str_echo(int);
#endif
