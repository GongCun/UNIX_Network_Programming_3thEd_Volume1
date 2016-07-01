#ifndef _PRAC_UDP_H
#define _PRAC_UDP_H

#include "unp.h"
#include <netinet/tcp.h>

void prac_dg_echo(int, SA *, socklen_t);
void prac_dg_echo02(int, SA *, socklen_t);
void prac_dg_cli(FILE *, int, const SA *, socklen_t);
void prac_dg_cli02(FILE *, int, const SA *, socklen_t);
void prac_dg_cli03(FILE *, int, const SA *, socklen_t);
void prac_dg_cli04(FILE *, int, const SA *, socklen_t);
void prac_dg_cli05(FILE *, int, const SA *, socklen_t);

void exer02_dg_echo(int, SA *, socklen_t);
void exer07_dg_cli(FILE *, int, const SA *, socklen_t);
void exer08_dg_cli(FILE *, int, const SA *, socklen_t);
void exer08_dg_echo(int, SA *, socklen_t);

#endif
