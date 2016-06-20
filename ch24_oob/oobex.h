#ifndef _OOBEX_H
#define _OOBEX_H

#include "unp.h"

void oob_heartbeat_cli(int, int, int);
void oob_heartbeat_serv(int, int, int);
void oob_str_cli(FILE *, int);
void oob_str_echo(int);

#endif
