include ../Make.defines
OS=_$(shell uname -s)
#CFLAGS += -D_DEBUG -D_THREAD_TEST -D$(OS) -D_DISC_DONT
#CFLAGS += -D_DEBUG -D$(OS) -D_DISC_DONT
#CFLAGS += -D$(OS) -D_DISC_DONT
CFLAGS += -D_THREAD_TEST -D$(OS) -D_DISC_DONT

PROGS =	rawping

all:	${PROGS}

rawping:	raw_main.o raw_ping.o 
		${CC} ${CFLAGS} -D_THREAD_TEST -o $@ $^ ${LIBS}

raw_ping.o:	raw_ping.c
		@echo checking header
		@sh ./check.sh
		@cat ./check.h
		${CC} ${CFLAGS} -D_THREAD_TEST -c -o $@ $^

clean:
		rm -r -f ${PROGS} ${CLEANFILES} *.dSYM
