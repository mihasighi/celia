# Makefile
#
# APRON Library / Shape Domain
#
# Copyright (C) LIAFA 2009

# This file is part of the APRON Library, released under LGPL license.
# Please read the COPYING file packaged in the distribution.

# CINV shall be defined
# DEBUG shall be defined

include $(CINV)/Makefile.config

#---------------------------------------
# Flags
#---------------------------------------

# Use ICFLAGS to specify machine-independent compilation flags.
ICFLAGS = \
-I$(MPFR_PREFIX)/include \
-I$(GMP_PREFIX)/include \
-I$(CAML_PREFIX)/lib/ocaml \
-I$(CAMLIDL_PREFIX)/lib/ocaml \
-I$(MLGMPIDL_PREFIX)/include \
-I$(APRON_PREFIX)/include \
-I$(APRON_PREFIX)/lib \
-I$(CINV)/shapes

#---------------------------------------
# Files
#---------------------------------------

CCSOURCES = addV_eq.c  initFibo.c  initMod2.c  initSeq.c  initV.c

CCTESTS = $(subst .c,.exe,$(CCSOURCES))

#---------------------------------------
# Rules
#---------------------------------------

root:
	@echo
	@echo "Please choose a target from:"
	@echo
	@echo " <prg-name>.exe          : prg-name is built"
	@echo " all                     : all programs are built"
	@echo " indent                  : to indent C sources"
	@echo " clean                   : remove objects"
	@echo

all: $(CCTESTS)

clean:
	/bin/rm -f *.[ao] *.exe *.shp

indent: $(CCSOURCES) 
	for i in $(CCSOURCES) ; do \
		echo "* indent $$i"; indent -bl -i2 $$i; \
	done

#-----------------------------------
# C part
#-----------------------------------

NDOM=MPQ

SHAPE_LIBS = \
-llsum \
-lmset \
-lucons \
-lshape$(DEBUG)

APRON_LIBS = \
-lapron$(DEBUG) \
-lbox$(NDOM)$(DEBUG) \
-loct$(NDOM)$(DEBUG) \
-lpolka$(NDOM)$(DEBUG) \
-lap_ppl$(DEBUG)

#---------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#---------------------------------------

.SUFFIXES: .c .h .o .exe

%.exe: %.o %_debug.o 
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -o $@ $*$(DEBUG).o \
	-L$(CINV)/shapes $(SHAPE_LIBS) \
	-L$(APRON_PREFIX)/lib $(APRON_LIBS) \
	-L$(MPFR_PREFIX)/lib -lmpfr \
	-L$(GMP_PREFIX)/lib -lgmpxx -lgmp \
	-lm

%.o: %.c $(CCINC) $(DEPS)
	$(CC) $(CFLAGS) $(ICFLAGS) -c -o $@ $<

%_debug.o: %.c $(CCINC) $(DEPS)
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -c -o $@ $<


#-----------------------------------
# DEPENDENCIES
#-----------------------------------
