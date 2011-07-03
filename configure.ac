#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([xargon], [1.0], [malvineous@shikadi.net])
AM_INIT_AUTOMAKE([xargon], [1.0])
AC_CONFIG_SRCDIR([src/_xargon.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_LANG(C++)

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
AC_PROG_LIBTOOL

PKG_CHECK_MODULES([sdl], [sdl])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AM_SILENT_RULES([yes])

AC_OUTPUT(Makefile src/Makefile)
