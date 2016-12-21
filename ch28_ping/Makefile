include ../Make.defines
OS=$(shell uname -s)
#CFLAGS += -D_DEBUG -D_THREAD_TEST -D_$(OS) -D_DISC_DONT
#CFLAGS += -D_DEBUG -D_$(OS) -D_DISC_DONT
#CFLAGS += -D_$(OS) -D_DISC_DONT
CFLAGS += -D_THREAD_TEST -D_$(OS) -D_DISC_DONT
VERSION = $(shell grep '\$$Id' raw_main.c | awk '{print $$4}')

PROGS =	rawping

all:	${PROGS}

rawping:	raw_main.o raw_ping.o 
		${CC} ${CFLAGS} -D_THREAD_TEST -o $@ $^ ${LIBS}

%.o:		%.c check.h version.h
		${CC} ${CFLAGS} -D_THREAD_TEST -c -o $@ $<

check.h:	raw_main.c
		@echo checking header
		@sh ./check.sh
		@cat ./check.h

version.h:	raw_main.c
		@echo building version
		@rm -f $@
		@echo 'fputs("Version ${VERSION} compile on $(OS) at $(shell date)\\n", stdout);' >>$@

clean:
		rm -r -f ${PROGS} ${CLEANFILES} *.dSYM
