#ifndef _SOCKQ_H
#define _SOCKQ_H

#include "../config.h"

#define KMEM "/dev/kmem"
#if (_AIX64)
# define FDSINFO fdsinfo64
# define PROCINFO procentry64
# define GETPROCS getprocs64
# define LSEEK lseek64
# define OFF_T off64_t
#else 
# define FDSINFO fdsinfo
# define PROCINFO procsinfo
# define GETPROCS getprocs
# define LSEEK lseek
# define OFF_T off_t
#endif

#define PROCSIZE    sizeof(struct PROCINFO)

#define FDSINFOSIZE sizeof(struct FDSINFO) /* (If we're lucky.) */
#if defined(OPEN_SHRT_MAX)
# if OPEN_SHRT_MAX<OPEN_MAX
#   undef  FDSINFOSIZE /* (We weren't lucky.) */
#   define FDSELEMSIZE (sizeof(struct FDSINFO)/OPEN_SHRT_MAX)
#   define FDSINFOSIZE (OPEN_MAX * FDSELEMSIZE)
# endif /* OPEN_SHRT_MAX<OPEN_MAX */
#endif  /* defined(OPEN_SHRT_MAX) */

typedef u_longlong_t KA_T; /* kernel memory address type */
extern int Kd; /* KMEM descriptor */
extern void err_sys(const char *fmt, ...);
extern void err_quit(const char *fmt, ...);
void procfile(KA_T);

#endif

