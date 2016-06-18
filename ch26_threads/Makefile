include ../Make.defines
CFLAGS := $(CFLAGS) -I.

PROGS =	main_cli main_serv thread_web_main

all:	${PROGS}

main_cli: main_cli.c thread_str_cli.c thread_readline.c
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

main_serv: main_serv.c thread_str_echo.c thread_readline.c
		${CC} ${CFLAGS} -o $@ $^ ${LIBS}

thread_web_main: thread_web_main.c thread_web.c
		${CC} ${CFLAGS} -o $@ $^ ${LIBS} 

clean:
		rm -f ${PROGS} ${CLEANFILES}
		rm -r -f *.dSYM
