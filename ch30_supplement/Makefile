include ../Make.defines

PROGS =	mode_serv02_dyn_macos mode_serv03m_dyn_linux

all:	${PROGS}

# serv02_dyn_macos: prefork, dynamic increase/decrease children# by
# monitoring the queue length of listenfd on Mac OS X
mode_serv02_dyn_macos:	mode_serv02_dyn_macos.c mode_reply.c mode_cpu_time.c mode_lock.c
		${CC} ${CFLAGS} -D_PREFORK -o $@ $^ ${LIBS}

# serv03m_dyn_linux: prefork, file locking using fcntl(), metered.
# Dynamic increase/decrease children# by monitoring
# the queue length of listenfd on Mac OS X
mode_serv03m_dyn_linux:    mode_serv03m_dyn_linux.o child03m.o lock_fcntl.o web_child.o pr_cpu_time.o meter.o
	        ${CC} ${CFLAGS} -o $@ serv03m.o child03m.o lock_fcntl.o web_child.o \
				            pr_cpu_time.o meter.o ${LIBS}


clean:
		rm -f ${PROGS} ${CLEANFILES}
		rm -r -f *.dSYM
