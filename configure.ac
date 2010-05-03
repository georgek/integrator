dnl This file is part of Integrator
dnl (C) 2010 George Kettleborough
dnl All rights reserved (at least until the project is finished)
dnl 2010 Computer science final year project

# prelude
AC_INIT([integrator], [1.0], [g.kettleborough@uea.ac.uk])
AM_INIT_AUTOMAKE([-Wall -Werror])

# check for programs: C compiler, lex and yacc
AC_PROG_CC
AM_PROG_LEX
AC_PROG_YACC

# check for GNU readline
AC_CHECK_LIB([readline], [readline])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
