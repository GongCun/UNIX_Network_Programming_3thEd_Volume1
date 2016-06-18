include ../Make.defines

PROGS =	rawsockux01 rawsockux_serv rawsockux_cli

all:	${PROGS}

rawsockux01: rawsockux01.c
		${CC} ${CFLAGS} -D_DEBUG -o $@ $^ ${LIBS}

rawsockux_serv: rawsockux_serv.c raw_icmpd.c
		sudo ${CC} ${CFLAGS} -D_DEBUG -D_BSD -o $@ $^ ${LIBS}
		sudo chown root $@
		sudo chmod u+s $@

rawsockux_cli: rawsockux_cli.c raw_icmpd.c
		${CC} ${CFLAGS} -D_DEBUG -o $@ $^ ${LIBS}

clean:
		sudo rm -f ${PROGS} ${CLEANFILES}
		sudo rm -f -r *.dSYM
