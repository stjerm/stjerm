STJERM_VERSION = 0.10

bin_PROGRAMS = stjerm

stjerm_SOURCES = main.c mainwindow.c config.c term.c popupmenu.c shortcut.c

INCLUDES= -Wall\
          -DSTJERM_VERSION=\"$(STJERM_VERSION)\"
		 
LIBS = @glib_LIBS@ @LIBS@\
       @gtk_LIBS@\
       @gthread_LIBS@\
       @vte_LIBS@
	     
AM_CFLAGS = @CFLAGS@\
	    @glib_CFLAGS@\
	    @gtk_CFLAGS@\
	    @gthread_CFLAGS@\
	    @vte_CFLAGS@
