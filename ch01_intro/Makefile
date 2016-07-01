include ../Make.defines

PROGS =	daytimetcpsrv daytimetcpcli

all:	${PROGS}

$(PROGS):	%: %.o	
		${CC} ${CFLAGS} -o $@ $< ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES}
