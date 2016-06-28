include ../Make.defines

PROGS =	

all:	sigio_udpcli sigio_udpserv

$(PROGS):   %: %.o
	        ${CC} ${CFLAGS} -o $@ $< ${LIBS}

sigio_udpcli: sigio_udpcli.o sigio_dg_cli.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

sigio_udpserv: sigio_udpserv.o sigio_dg_echo.o
		${CC} ${CFLAGS} -o $@ $^  ${LIBS}
clean:
		rm -f ${PROGS} ${CLEANFILES} sigio_udpcli sigio_udpserv
