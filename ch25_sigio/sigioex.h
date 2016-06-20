#ifndef _SIGIO_EX_H
#define _SIGIO_EX_H

#include "unp.h"

void sigio_dg_echo(int, SA *, socklen_t);
void sigio_dg_cli(FILE *, int, SA *, socklen_t);

#endif

