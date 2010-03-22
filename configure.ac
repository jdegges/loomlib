#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.64])
AC_INIT([loomlib], [dev], [jdegges@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for header/libraries files.
AC_CHECK_HEADERS([stdlib.h stddef.h])

ACX_PTHREAD([AC_SUBST([CC], ["${PTHREAD_CC}"])
             AC_DEFINE([HAVE_PTHREAD], [1])],
    [AC_MSG_ERROR(['libpthread' not found])])


# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_HEADER_STDBOOL

AC_CONFIG_FILES([Makefile
                 loomlib/Makefile])
AC_OUTPUT
