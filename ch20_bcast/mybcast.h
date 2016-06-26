#ifndef _MY_BCAST_H
#define _MY_BCAST_H

#include "unp.h"

void dg_cli_bcast(FILE *, int, const SA *, socklen_t);
void dg_cli_bcast_err(FILE *, int, const SA *, socklen_t);
void dg_cli_bcast_jmp(FILE *, int, const SA *, socklen_t);
void dg_cli_bcast_ipc(FILE *, int, const SA *, socklen_t);
void dg_cli_bcast_batch(FILE *, int, const SA *, socklen_t);

#endif
