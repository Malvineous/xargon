#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([xargon], [1.0],
	[https://github.com/Malvineous/xargon/issues])
AM_INIT_AUTOMAKE([foreign dist-bzip2 no-dist-gzip])
AC_CONFIG_SRCDIR([src/_xargon.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_LANG(C++)

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
AC_PROG_LIBTOOL

PKG_CHECK_MODULES([sdl], [sdl])
PKG_CHECK_MODULES([libgamemusic], [libgamemusic], [HAVE_MUSIC=1], [HAVE_MUSIC=0])

AM_CONDITIONAL([USE_MUSIC], [test "$HAVE_MUSIC" -eq 1])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AM_SILENT_RULES([yes])

AC_OUTPUT(Makefile src/Makefile)
