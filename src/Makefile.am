STJERM_VERSION = 0.14

bin_PROGRAMS = stjerm

stjerm_SOURCES = main.cc StjermWindow.cc

INCLUDES= -Wall\
          @CFLAGS@\
	      @glib_CFLAGS@\
	      @gtkmm_CFLAGS@\
	      @gthread_CFLAGS@\
	      @vte_CFLAGS@\
          -DSTJERM_VERSION=\"$(STJERM_VERSION)\"
		 
LIBS = @glib_LIBS@ @LIBS@\
       @gtkmm_LIBS@\
       @gthread_LIBS@\
       @vte_LIBS@

