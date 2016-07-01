include ../Make.defines

PROGS =	serv_main serv_main_poll cli_main_pause serv_main_exer

all:	${PROGS} cli_main 

$(PROGS):	%: %.o	
		${CC} ${CFLAGS} -o $@ $< ${LIBS}

cli_main: cli_main.o str_cli_select.o 
		${CC} ${CFLAGS} -o $@ cli_main.o str_cli_select.o ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES} cli_main
