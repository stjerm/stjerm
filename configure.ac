# Process this file with autoconf to produce a configure script.
# some parts are taken from configure.ac from gmpc-svn
# (see: sarine.nl/gmpc)

AC_INIT([stjerm C], [0.18], [kristopherwilson@gmail.com],
    [main_c], [http://github.com/stjerm/])
AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([1.10 no-define foreign])
AC_CONFIG_SRCDIR([src/mainwindow.c])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_SUBST(GLIB_REQUIRED)
AC_SUBST(GTK_REQUIRED)

# glib
PKG_CHECK_MODULES([glib], glib-2.0 >= 2.32)
AC_SUBST(glib_LIBS)
AC_SUBST(glib_CFLAGS)

# gtk
PKG_CHECK_MODULES([gtk], gtk+-2.0 >= 2.24)
AC_SUBST(gtk_LIBS)
AC_SUBST(gtk_CFLAGS)

# gthread
PKG_CHECK_MODULES([gthread], gthread-2.0)
AC_SUBST(gthread_LIBS)
AC_SUBST(gthread_CFLAGS)

# vte
PKG_CHECK_MODULES([vte], vte >= 0.28)
AC_SUBST(vte_LIBS)
AC_SUBST(vte_CFLAGS)

# Checks for Xinerama extension
PKG_CHECK_MODULES([XINERAMA], [xinerama], [ac_have_xinerama="yes"], [
AC_CHECK_LIB(Xinerama, XineramaQueryExtension, 
    XINERAMA_LIBS="-lXinerama"
    ac_have_xinerama="yes",,
    $X_LIBS $X_PRE_LIBS -lXext $X_EXTRA_LIBS)
])

# Checks for header files.
AC_PATH_X
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T

# Checks for library functions.
AC_TYPE_SIGNAL
AC_FUNC_STAT

AC_OUTPUT([Makefile doc/Makefile src/Makefile])

echo "ready. type make to build..."
