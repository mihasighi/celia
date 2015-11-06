# Automake source file for Cinv.
# Copyright (C) 2009 Mihaela Sighireanu <mihaela.sighireanu@liafa.jussieu.fr>
#
# Tell aclocal where to find `.m4' files.
#ACLOCAL_AMFLAGS = -I m4

DISTCHECK_CONFIGURE_FLAGS = \
--enable-shared --enable-watchdog

dist_noinst_SCRIPTS = \
configure

EXTRA_DIST = \
cinv.spec.in \
BUGS CREDITS README 

@SET_MAKE@

SUBDIRS = src shapes frama-c-plugin samples doc 

