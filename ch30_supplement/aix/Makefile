CC = xlc
CFLAGS = -g -q64

PROGS = sockq

all:	$(PROGS)

sockq:	sockq.o procfile.o
		$(CC) $(CFLAGS) -o $@ $^

sockq.o:	sockq.c
		$(CC) $(CFLAGS) -c -o $@ $^

procfile.o:	procfile.c
		$(CC) $(CFLAGS) -D_KERNEL -c -o $@ $^

clean:
		rm -f $(PROGS) core *.o
