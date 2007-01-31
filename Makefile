include config.mk

SRC = main.c
OBJ = ${SRC:.c=.o}

all: stjerm

.c.o:
	@${CC} -c ${STJERMCFLAGS} $<

stjerm: ${OBJ}
	@${LD} -o $@ ${OBJ} ${STJERMLDFLAGS}
	@strip $@

clean:
	@rm -f stjerm ${OBJ}

