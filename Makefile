STJERM_VERSION = 0.8a

INCS = -I. -I/usr/include `pkg-config --cflags gtk+-2.0 vte` \
	   -DSTJERM_VERSION=\"${STJERM_VERSION}\" ${CFLAGS}
LIBS = -L/usr/lib `pkg-config --libs gtk+-2.0 vte gthread-2.0` ${LDFLAGS}

SRC = main.c mainwindow.c term.c popupmenu.c config.c shortcut.c
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

install: all
	@mkdir -p ${DESTDIR}/usr/bin
	@echo cp stjerm ${DESTDIR}/usr/bin/stjerm
	@cp stjerm ${DESTDIR}/usr/bin/stjerm
	@echo chmod 755 ${DESTDIR}/usr/bin/stjerm
	@chmod 755 ${DESTDIR}/usr/bin/stjerm

uninstall:
	@echo rm -f ${DESTDIR}/usr/bin/stjerm
	@rm -f ${DESTDIR}/usr/bin/stjerm
	
.PHONY: all clean install uninstall

