# Process this file with autoconf to produce a configure script.
# some parts are taken from configure.ac from gmpc-svn
# (see: sarine.nl/gmpc)
AC_INIT(src)
AC_PREREQ(2.59)
AM_INIT_AUTOMAKE([stjerm], [0.14], kristopherwilson@gmail.com)
#AC_INIT(FULL-PACKAGE-NAME, VERSION, BUG-REPORT-ADDRESS)
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
AC_SUBST(GLIB_REQUIRED)
AC_SUBST(GTK_REQUIRED)

# glib
PKG_CHECK_MODULES([glib], glib-2.0 >= 2.10)
AC_SUBST(glib_LIBS)
AC_SUBST(glib_CFLAGS)

# gthread
PKG_CHECK_MODULES([gthread], gthread-2.0)
AC_SUBST(gthread_LIBS)
AC_SUBST(gthread_CFLAGS)

PKG_CHECK_MODULES([gtkmm], [gtkmm-2.4 >= 2.12.2])
AC_SUBST(gtkmm_LIBS)
AC_SUBST(gtkmm_CFLAGS)

# gthread
PKG_CHECK_MODULES([vte], vte >= 0.20)
AC_SUBST(vte_LIBS)
AC_SUBST(vte_CFLAGS)

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
