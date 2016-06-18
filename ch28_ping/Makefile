include ../Make.defines

PROGS =	rawping

all:	${PROGS}

rawping:	raw_main.c raw_ping.c 
		sudo ${CC} ${CFLAGS} -D_DEBUG -D_THREAD_TEST -o $@ $^ ${LIBS}
		sudo chown root $@
		sudo chmod u+s $@

clean:
		sudo rm -f ${PROGS} ${CLEANFILES}
		sudo rm -f -r *.dSYM
