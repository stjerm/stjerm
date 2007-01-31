# version
STJERM_VERSION = 0.1svn

# paths
PREFIX = /usr
CONFPREFIX = ${PREFIX}/etc
MANPREFIX = ${PREFIX}/share/man

# includes and libs
INCS = -I. -I${PREFIX}/include -I/usr/include -I/usr/include/gtk-2.0 \
-I/usr/lib/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/cairo \
-I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include \
-I/usr/include/freetype2 -I/usr/include/libpng12
LIBS = -L/usr/lib -L${PREFIX}/lib -lvte -lncurses -lXft -lgtk-x11-2.0 \
-lpangoxft-1.0 -lpangox-1.0 -lXrender -lfontconfig -lfreetype -lz -lX11 \
-lgdk-x11-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lcairo \
-lpangoft2-1.0 -lpango-1.0 -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0

# flags
STJERMCFLAGS = ${INCS} -DSTJERM_VERSION=\"${STJERM_VERSION}\" ${CFLAGS}
STJERMLDFLAGS = ${LIBS} ${LDFLAGS} 

# compiler and linker
CC = cc
LD = ${CC}
