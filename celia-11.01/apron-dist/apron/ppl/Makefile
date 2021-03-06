# Makefile
#
# APRON Library / PPL library wrapper
#
# Copyright (C) Antoine Mine' 2006

# This file is part of the APRON Library, released under GPL license.
# Please read the COPYING file packaged in the distribution.

include ../Makefile.config

PREFIX = $(APRON_PREFIX)

# C include and lib directories
INCDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib
BINDIR = $(PREFIX)/bin
CAMLDIR = $(PREFIX)/lib

SRCDIR = $(shell pwd)

#---------------------------------------
# Programs
#---------------------------------------

#---------------------------------------
# Flags
#---------------------------------------

# Use ICFLAGS to specify machine-independent compilation flags.
ICFLAGS = \
-I$(MLGMPIDL_PREFIX)/include \
-I../newpolka \
-I../apron \
-I../mlapronidl \
-I../num \
-I../itv \
-I$(GMP_PREFIX)/include -I$(MPFR_PREFIX)/include \
-I$(PPL_PREFIX)/include \
-I$(CAML_PREFIX)/lib/ocaml -I$(CAMLIDL_PREFIX)/lib/ocaml

ICXXFLAGS = \
-I../newpolka \
-I$(MLGMPIDL_PREFIX)/include \
-I../apron \
-I../mlapronidl \
-I../num \
-I../itv \
-I$(GMP_PREFIX)/include -I$(MPFR_PREFIX)/include \
-I$(PPL_PREFIX)/include \
-I$(CAML_PREFIX)/lib/ocaml -I$(CAMLIDL_PREFIX)/lib/ocaml

# Caml
OCAMLINC = -I $(MLGMPIDL_PREFIX)/lib -I ../mlapronidl

#---------------------------------------
# Files
#---------------------------------------

CXXSOURCES = ppl_user.cc ppl_poly.cc ppl_grid.cc
CSOURCES = ppl_test.c
CCINC = ppl_user.hh ppl_poly.hh ppl_grid.hh ppl_grid.h ap_ppl.h

#---------------------------------------
# Rules
#---------------------------------------

all: libap_ppl.a libap_ppl_debug.a ap_ppl_test
ifneq ($(HAS_SHARED),)
all: libap_ppl.so libap_ppl_debug.so
endif

mlexample.byte: mlexample.ml ppl.cma
	$(OCAMLC) $(OCAMLFLAGS) -I $(MLGMPIDL_INSTALL)/lib -I $(APRON_PREFIX)/lib -o $@ bigarray.cma gmp.cma apron.cma ppl.cma $<

mlexample.opt: mlexample.ml ppl.cmxa
	$(OCAMLOPT) $(OCAMLOPTFLAGS) -I $(MLGMPIDL_INSTALL)/lib -I $(APRON_PREFIX)/lib -o $@ bigarray.cmxa gmp.cmxa apron.cmxa ppl.cmxa $<

clean:
	/bin/rm -f *.[ao] *.so ap_ppl_test
	/bin/rm -f *.cm[ioax] *.cmxa pplrun ppltop manager.idl
	/bin/rm -fr *~ \#*\# tmp

mostlyclean:
	/bin/rm -f ap_ppl_caml.c ppl.ml ppl.mli

install:
	$(INSTALLd) $(INCDIR) $(LIBDIR) $(BINDIR) $(CAMLDIR)
	$(INSTALL) ap_ppl.h $(INCDIR)
	$(INSTALL) libap_ppl.so libap_ppl_debug.so $(LIBDIR)
	$(INSTALL) libap_ppl.a libap_ppl_debug.a $(LIBDIR)
	for i in libap_ppl_caml*.a libap_ppl_caml*.so ppl.idl ppl.cmi ppl.cma ppl.cmxa ppl.a; do \
		if test -f $$i; then $(INSTALL) $$i $(CAMLDIR); fi; \
	done
	for i in dllap_ppl*.so; do \
		if test -f $$i; then cp -f -d $$i $(CAMLDIR); fi; \
	done	
	for i in ap_ppl_test pplrun ppltop; do \
		if test -f $$i; then $(INSTALL) $$i $(BINDIR); fi; \
	done

uninstall:
	/bin/rm -f $(INCDIR)/ap_ppl*
	/bin/rm -f $(LIBDIR)/libap_ppl* $(LIBDIR)/dllap_ppl* $(LIBDIR)/ppl.a $(LIBDIR)/ppl.idl $(LIBDIR)/ppl.cm*
	/bin/rm -f $(BINDIR)/*ap_ppl*
	/bin/rm -f $(BINDIR)/pplrun $(BINDIR)/ppltop

distclean: uninstall

dist: Makefile COPYING README $(CXXSOURCES) $(CSOURCES) $(CCINC) ppl.idl sedscript_caml ap_ppl_caml.c ppl.ml ppl.mli ppl.patch
	(cd ..; tar zcvf ppl.tgz $(^:%=ppl/%))

#---------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#---------------------------------------

.SUFFIXES: .tex .cc .h .a .o .so

#-----------------------------------
# C / C++ part
#-----------------------------------

libap_ppl.a: $(subst .cc,.o,$(CXXSOURCES))
	$(AR) rcs $@ $^
	$(RANLIB) $@
libap_ppl_debug.a: $(subst .cc,_debug.o,$(CXXSOURCES))
	$(AR) rcs $@ $^
	$(RANLIB) $@
libap_ppl.so: $(subst .cc,.o,$(CXXSOURCES))
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ -lppl -lgmpxx -L$(PPL_PREFIX)/lib -L$(GMP_PREFIX)/lib -Wl,-rpath,$(PPL_PREFIX)/lib:$(APRON_PREFIX)/lib:$(GMP_PREFIX)/lib
libap_ppl_debug.so: $(subst .cc,_debug.o,$(CXXSOURCES))
	$(CXX) $(CXXFLAGS_DEBUG) -shared -o $@ $^ -lppl -lgmpxx -L$(PPL_PREFIX)/lib -L$(GMP_PREFIX)/lib -Wl,-rpath,$(PPL_PREFIX)/lib:$(APRON_PREFIX)/lib:$(GMP_PREFIX)/lib

ap_ppl_test: libap_ppl_debug.a ppl_test_debug.o
	$(CXX) $(CXXFLAGS) -o $@ ppl_test_debug.o \
	-L. -lap_ppl_debug \
	-L../newpolka -L$(APRON_PREFIX)/lib -lpolkaMPQ_debug \
	-L../apron -L$(APRON_PREFIX)/lib -lapron \
	-L$(PPL_PREFIX)/lib -lppl \
	-L$(GMP_PREFIX)/lib -L$(MPFR_PREFIX)/lib -lgmpxx -lmpfr -lgmp -lm

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(ICXXFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -c -o $@ $<

%_debug.o: %.cc
	$(CXX) $(CXXFLAGS_DEBUG) $(ICXXFLAGS) -c -o $@ $<

%_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -c -o $@ $<

#-----------------------------------
# Caml part
#-----------------------------------

ml: ppl.cma ppl.cmxa libap_ppl_caml.a libap_ppl_caml_debug.a

ifneq ($(HAS_SHARED),)
ml: libap_ppl_caml.so libap_ppl_caml_debug.so dllap_ppl_caml.so dllap_ppl_caml_debug.so
endif

#ppltop: ppl.cma libap_ppl_caml.a libap_ppl.a
#	$(OCAMLMKTOP) $(OCAMLINC) $(OCAMLFLAGS) -o $@ -cc $(CXX) -custom \
#	bigarray.cma gmp.cma apron.cma ppl.cma \
#	-ccopt "-L. -L../apron  -L../itv -L$(MLGMPIDL_PREFIX)/lib -L../mlapronidl"

#pplrun: ppl.cma libap_ppl_caml.a
#	$(OCAMLC) $(OCAMLINC) $(OCAMLFLAGS) -o $@ -make-runtime -cc $(CXX) \
#	bigarray.cma gmp.cma apron.cma ppl.cma \
#	-ccopt "-L.  -L../apron -L../itv -L$(MLGMPIDL_PREFIX)/lib -L../mlapronidl"

libap_ppl_caml.a: ap_ppl_caml.o
	$(AR) rcs $@ $^
	$(RANLIB) $@
libap_ppl_caml_debug.a: ap_ppl_caml_debug.o
	$(AR) rcs $@ $^
	$(RANLIB) $@
libap_ppl_caml.so: ap_ppl_caml.o libap_ppl.so
	$(CXX) $(CXXFLAGS) -shared -o $@ $< -lap_ppl -lppl -lgmpxx -L. -L$(PPL_PREFIX)/lib -L$(GMP_PREFIX)/lib -Wl,-rpath,$(PPL_PREFIX)/lib:$(APRON_PREFIX)/lib
libap_ppl_caml_debug.so: ap_ppl_caml_debug.o libap_ppl_debug.so
	$(CXX) $(CXXFLAGS) -shared -o $@ $< -lap_ppl_debug -lppl -lgmpxx -L. -L$(PPL_PREFIX)/lib -L$(GMP_PREFIX)/lib -Wl,-rpath,$(PPL_PREFIX)/lib:$(APRON_PREFIX)/lib
dllap_ppl_caml.so: libap_ppl_caml.so
	ln -s -f $^ $@
dllap_ppl_caml_debug.so: libap_ppl_caml_debug.so
	ln -s -f $^ $@

ppl.cma: ppl.cmo libap_ppl_caml.a
	$(OCAMLMKLIB) -ocamlc "$(OCAMLC) -cc $(CXX)" -verbose -o ppl -oc ap_ppl_caml ppl.cmo -lap_ppl -lppl -lgmpxx -L$(PPL_PREFIX)/lib -L$(APRON_PREFIX)/lib -L$(GMP_PREFIX)/lib -Wl,-rpath,$(PPL_PREFIX)/lib:$(APRON_PREFIX)/lib:$(GMP_PREFIX)/lib

ppl.cmxa: ppl.cmx libap_ppl_caml.a
	$(OCAMLMKLIB) -ocamlopt "$(OCAMLOPT) -cc $(CXX)" -verbose -o ppl -oc ap_ppl_caml ppl.cmx -lap_ppl -lppl -lgmpxx -L$(PPL_PREFIX)/lib -L$(APRON_PREFIX)/lib -L$(GMP_PREFIX)/lib -Wl,-rpath,$(PPL_PREFIX)/lib:$(APRON_PREFIX)/lib:$(GMP_PREFIX)/lib

manager.idl: ../mlapronidl/manager.idl
	ln -s $< $@

rebuild: manager.idl ppl.idl
	mkdir -p tmp
	cp ppl.idl tmp/
	$(CAMLIDL) -no-include -nocpp tmp/ppl.idl
	cp tmp/ppl_stubs.c ap_ppl_caml.c
	sed -f sedscript_caml tmp/ppl.ml >ppl.ml
	sed -f sedscript_caml tmp/ppl.mli >ppl.mli

.PRECIOUS: %_caml.c %.ml %.mli %.cmi libap_ppl_caml.a libap_ppl_caml.so ppl.cmx ppl.cmo


#---------------------------------------
# ML generic rules
#---------------------------------------

%.cmi: %.mli  $(DEPS)
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmo: %.ml %.cmi  $(DEPS)
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmx: %.ml %.cmi  $(DEPS)
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -c $<
