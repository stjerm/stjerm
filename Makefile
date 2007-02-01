STJERM_VERSION = 0.1svn

STJERMPREFIX = $(shell if [ ! -z $(PREFIX) ]; then echo $(PREFIX); else \
echo "/usr/local"; fi)

INCS = -I. -I${PREFIX}/include `pkg-config --cflags gtk+-2.0 vte`
LIBS = -L${PREFIX}/lib `pkg-config --libs gtk+-2.0 vte`
STJERMCFLAGS = ${INCS} -DSTJERM_VERSION=\"${STJERM_VERSION}\" ${CFLAGS}
STJERMLDFLAGS = ${LIBS} ${LDFLAGS} 

CC = cc
LD = ${CC}

SRC = main.c
OBJ = ${SRC:.c=.o}

all: options stjerm
	
options:
	@echo Build options:
	@echo "PREFIX   = $(STJERMPREFIX)"
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo
	
.c.o:
	@echo Building...
	@echo ${CC} -Wall -c ${STJERMCFLAGS} $<
	@${CC} -Wall -c ${STJERMCFLAGS} $<
	@echo
	
stjerm: ${OBJ}
	@echo ${LD} -o $@ ${OBJ} ${STJERMLDFLAGS}
	@${LD} -o $@ ${OBJ} ${STJERMLDFLAGS}
	@strip $@
	@echo
	
clean:
	@echo Cleaning...
	@echo rm -f stjerm ${OBJ}
	@rm -f stjerm ${OBJ}
	@echo
	
.PHONY: all options clean
