#CC = gcc
PLATFORM = $(shell ./platform.sh | cut -d'%' -f1)

CFLAGS = $(shell ./platform.sh | cut -d'%' -f2) -g

PROGS = sockq

all: $(PROGS)

$(PROGS): *.c $(PLATFORM)/*.c
	(cd $(PLATFORM) && $(MAKE) CFLAGS="$(CFLAGS)") && ln -s -f $(PLATFORM)/$(PROGS) .

clean:
	rm -f $(PROGS) $(PLATFORM)/$(PROGS) $(PLATFORM)/*.o
