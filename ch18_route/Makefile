include ../Make.defines

PROGS =	checkudpsum

all:	${PROGS} getrt prifinfo

$(PROGS):   %: %.o
	        ${CC} ${CFLAGS} -o $@ $< ${LIBS}

getrt:	getrt.o get_rtaddrs.o sock_masktop.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

prifinfo:	prifinfo.o get_ifi_info.o free_ifi_info.o
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES} getrt prifinfo
