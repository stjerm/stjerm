STJERM_VERSION = 0.1svn

INCS = -I. -I/usr/include `pkg-config --cflags gtk+-2.0 vte` \
	   -DSTJERM_VERSION=\"${STJERM_VERSION}\" ${CFLAGS}
LIBS = -L/usr/lib `pkg-config --libs gtk+-2.0 vte gthread-2.0` ${LDFLAGS}

SRC = main.c mainwindow.c term.c popupmenu.c config.c grabkey.c
OBJ = ${SRC:.c=.o}

all: stjerm
	
.c.o:
	@echo cc -Wall -c ${INCS} $<
	@cc -Wall -c ${INCS} $<
	@echo
	
stjerm: ${OBJ}
	@echo cc -o $@ ${OBJ} ${LIBS}
	@cc -o $@ ${OBJ} ${LIBS}
	@echo
	
clean:
	@echo rm -f stjerm ${OBJ}
	@rm -f stjerm ${OBJ}
	@echo
	
.PHONY: all options clean

