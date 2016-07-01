#ifndef _SOCKQ_H
#define _SOCKQ_H

/* #include "../config.h" */
#include <libproc.h>
#include <sys/proc_info.h>

extern void err_sys(const char *fmt, ...);
extern void err_quit(const char *fmt, ...);

#endif
