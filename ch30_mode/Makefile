include ../Make.defines

PROGS =	mode_client mode_serv00 mode_serv01 mode_serv02 mode_serv02_dyn \
		mode_serv02_dyn_v02 mode_serv03 mode_serv04 mode_serv04b mode_serv05 \
		mode_serv05_dyn mode_serv06 mode_serv07 mode_serv07_dyn mode_serv08 \
		ex01 ex02 mode_serv02_dyn_v03

all:	${PROGS}

# The client to test the various servers.
mode_client:	mode_client.c mode_connect.c
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

# serv00: traditional concurrent server: use as base level
mode_serv00:	mode_serv00.c mode_reply.c mode_cpu_time.c sig_int.c
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

# serv01: one fork per client (traditional concurrent server).
mode_serv01:	mode_serv01.c mode_reply.c mode_cpu_time.c sig_int.c sig_chld.c
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

# serv02: prefork, no locking; works on BSD-derived systems
#	but not on SVR4-derived systems.
mode_serv02:	mode_serv02.c mode_reply.c mode_cpu_time.c sig_int.c
		${CC} ${CFLAGS} -D_PREFORK -o $@ $^ ${LIBS}

# serv02_dyn: prefork, dynamic increase/decrease children# by pipe()
mode_serv02_dyn:	mode_serv02_dyn.c mode_reply.c mode_cpu_time.c mode_lock.c
		${CC} ${CFLAGS} -D_PREFORK -o $@ $^ ${LIBS}

# serv02_dyn_v02: prefork, dynamic increase/decrease children# by
# monitoring the queue length of listenfd.
mode_serv02_dyn_v02:	mode_serv02_dyn_v02.c mode_reply.c mode_cpu_time.c mode_lock.c
		${CC} ${CFLAGS} -D_PREFORK -o $@ $^ ${LIBS}

# Test use recv() or ioctl(FIONREAD) to see the listenfd's queue, _but_
# it's only useful in accept-fd.
mode_serv02_dyn_v03:	mode_serv02_dyn_v03.c mode_reply.c mode_cpu_time.c mode_lock.c
		${CC} ${CFLAGS} -D_PREFORK -o $@ $^ ${LIBS}

# prefork, no locking; parent close the listenfd
ex01:	ex01.c mode_reply.c mode_cpu_time.c sig_int.c
		${CC} ${CFLAGS} -D_PREFORK -o $@ $^ ${LIBS}

# serv02l: prefork, no locking, block in select instead of accept to see
#	select collisions; works on BSD-derived systems but not on SVR4.

# serv02m: prefork, no locking; works on BSD-derived systems.
#	This version is "metered" to see #clients/child serviced.

# serv03: prefork, file locking using fcntl().  Similar to Apache server, metered
mode_serv03:	mode_serv03.c mode_reply.c mode_cpu_time.c sig_int.c mode_lock.c
		${CC} ${CFLAGS} -D_PREFORK -o $@ $^ ${LIBS}

# serv03m: prefork, file locking using fcntl(), metered.

# serv04: prefork, file locking using semaphore locking.
mode_serv04:	mode_serv04.c mode_reply.c mode_cpu_time.c sig_int.c mode_lock.c
		${CC} ${CFLAGS} -D_PREFORK -o $@ $^ ${LIBS}
		
# serv04b: prefork, file locking using thread locking.
mode_serv04b:	mode_serv04b.c mode_reply.c mode_cpu_time.c sig_int.c mode_lock.c
		${CC} ${CFLAGS} -D_PREFORK -o $@ $^ ${LIBS}

# serv05: prefork, descrptor passing to children.  Similar to NSCA server.
mode_serv05:	mode_serv05.c mode_reply.c mode_cpu_time.c sig_int.c mode_read_fd.c mode_write_fd.c
		${CC} ${CFLAGS} -D_PASSFD -o $@ $^ ${LIBS}

ex02:	ex02.c mode_reply.c mode_cpu_time.c sig_int.c mode_read_fd.c mode_write_fd.c
		${CC} ${CFLAGS} -D_PASSFD -D__DYNAMIC -o $@ $^ ${LIBS}

# serv05_dyn: prefork, passing fd, with dynamic increase/decrease children processes
mode_serv05_dyn:	mode_serv05_dyn.c mode_reply.c mode_cpu_time.c mode_read_fd.c mode_write_fd.c mode_lock.c
		${CC} ${CFLAGS} -D_PASSFD -D__DYNAMIC -o $@ $^ ${LIBS}

# Thread versions must call a reentrant version of readline().
# serv06: one thread per client.
mode_serv06:	mode_serv06.c mode_reply.c mode_cpu_time.c sig_int.c mode_readline_r.c
		#${CC} ${CFLAGS} -D__PTHREAD -D_DEBUG -o $@ $^ ${LIBS}
		${CC} ${CFLAGS} -D__PTHREAD -o $@ $^ ${LIBS}

# serv07: prethread with mutex locking around accept().
mode_serv07:	mode_serv07.c mode_reply.c mode_cpu_time.c sig_int.c mode_readline_r.c
		${CC} ${CFLAGS} -D__PTHREAD -D_PRETHREAD -o $@ $^ ${LIBS}

# serv07_dyn: prethread with dynmaic increase/decrease the threads.
mode_serv07_dyn:	mode_serv07_dyn.c mode_reply.c mode_cpu_time.c sig_int.c mode_readline_r.c wrappthread.c
		${CC} ${CFLAGS} -D__PTHREAD -D_PRETHREAD -o $@ $^ ${LIBS}

# serv08: prethread with only main thread doing accept().
mode_serv08:	mode_serv08.c mode_reply.c mode_cpu_time.c sig_int.c mode_readline_r.c
		${CC} ${CFLAGS} -D__PTHREAD -D_PRETHREAD -o $@ $^ ${LIBS}

# serv09: prethread with no locking around accept().

clean:
		rm -f ${PROGS} ${CLEANFILES}
		rm -r -f *.dSYM
