include ../Make.defines
CFLAGS += -D_DEBUG -D_THREAD_TEST

PROGS =	rawping

all:	${PROGS}

rawping:	raw_main.o raw_ping.o 
		${CC} ${CFLAGS} -D_THREAD_TEST -o $@ $^ ${LIBS}

clean:
		rm -r -f ${PROGS} ${CLEANFILES} *.dSYM
