include ../Make.defines

PLATFORM := $(shell platform=$$(uname -s); \
				[ $$platform = "AIX" -o $$platform = "Darwin" ] && echo _BSD || echo _LINUX)

HOSTNAME := $(shell hostname)

CFLAGS := $(CFLAGS) -D$(PLATFORM) -D_CLIENT_CLEAN_SSR -D_SERVER_CLEAN_SSR

PROGS =	ipopts_tcpcli_main.$(HOSTNAME) ipopts_tcpserv_main.$(HOSTNAME)

all:	${PROGS}

ipopts_tcpcli_main.$(HOSTNAME): ipopts_tcpcli_main.c ipopts_sourceroute.c ipopts_str_cli.c
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

ipopts_tcpserv_main.$(HOSTNAME): ipopts_tcpserv_main.c ipopts_sourceroute.c ipopts_str_echo.c
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES}
		rm -f -r *.dSYM
