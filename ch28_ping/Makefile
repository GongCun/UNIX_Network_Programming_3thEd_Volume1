include ../Make.defines
OS=_$(shell uname -s)
#CFLAGS += -D_DEBUG -D_THREAD_TEST -D$(OS)
#CFLAGS += -D_DEBUG -D$(OS)
#CFLAGS += -D$(OS)
#CFLAGS += -D_THREAD_TEST -D$(OS)
CFLAGS += -D_THREAD_TEST -D$(OS) -D_DISC_DONT

PROGS =	rawping

all:	${PROGS}

rawping:	raw_main.o raw_ping.o 
		${CC} ${CFLAGS} -D_THREAD_TEST -o $@ $^ ${LIBS}

clean:
		rm -r -f ${PROGS} ${CLEANFILES} *.dSYM
