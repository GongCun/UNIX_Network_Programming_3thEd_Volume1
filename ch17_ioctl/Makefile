include ../Make.defines

PROGS =	prifinfo prmac

all:	${PROGS}

prifinfo:   prifinfo.o get_ifi_info.o free_ifi_info.o
	        ${CC} ${CFLAGS} -o $@ $^ ${LIBS}

prmac:   prmac.o get_ifi_info.o 
	        ${CC} ${CFLAGS} -o $@ $^ ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES}
