include ../Make.defines

PROGS =	rawtracert

all:	${PROGS}

rawtracert:	raw_main.c raw_tracert.c 
		sudo ${CC} ${CFLAGS} -D_DEBUG -o $@ $^ ${LIBS}
		sudo chown root $@
		sudo chmod u+s $@

clean:
		sudo rm -f ${PROGS} ${CLEANFILES}
		sudo rm -f -r *.dSYM
