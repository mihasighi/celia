include ../Makefile.config

PREFIX = $(APRON_PREFIX)

SRCDIR = $(shell pwd)

#---------------------------------------
# Flags
#---------------------------------------

# Use ICFLAGS to specify machine-independent compilation flags.
ICFLAGS = \
-I$(MLGMPIDL_PREFIX)/include \
-I../apron \
-I../num -I../itv \
-I../newpolka \
-I../ppl \
-I../mlapronidl \
-I$(GMP_PREFIX)/include -I$(MPFR_PREFIX)/include \
-I$(CAMLIDL_PREFIX)/lib/ocaml -I$(CAML_PREFIX)/lib/ocaml

# Caml
OCAMLINC = -I $(MLGMPIDL_PREFIX)/lib -I ../mlapronidl -I ../newpolka -I ../ppl

#---------------------------------------
# Files
#---------------------------------------

CCMODULES = ap_pkgrid
CCSRC = $(CCMODULES:%=%.h) $(CCMODULES:%=%.c)

CCINC_TO_INSTALL = ap_pkgrid.h
CCBIN_TO_INSTALL =
CCLIB_TO_INSTALL = \
libap_pkgrid.a libap_pkgrid_debug.a \
libap_pkgrid.so libap_pkgrid_debug.so \
polkaGrid.mli polkaGrid.ml \
polkaGrid.cmi polkaGrid.cmx polkaGrid.cma polkaGrid.cmxa polkaGrid.a \
libpolkaGrid_caml.a libpolkaGrid_caml_debug.a \
libpolkaGrid_caml.so libpolkaGrid_caml_debug.so

#---------------------------------------
# Rules
#---------------------------------------

all: libap_pkgrid.a libap_pkgrid_debug.a
ifneq ($(HAS_SHARED),)
all: libap_pkgrid.so libap_pkgrid_debug.so
endif

ml: polkaGrid.mli polkaGrid.ml polkaGrid.cmi polkaGrid.cmx polkaGrid.cma polkaGrid.cmxa libpolkaGrid_caml.a libpolkaGrid_caml_debug.a
ifneq ($(HAS_SHARED),)
ml: libpolkaGrid_caml.so libpolkaGrid_caml_debug.so dllpolkaGrid_caml.so dllpolkaGrid_caml_debug.so
endif

mlexample%.byte: mlexample.ml $(APRON_PREFIX)/lib/polka%.cma $(APRON_PREFIX)/lib/ppl.cma polkaGrid.cma
	$(OCAMLC) $(OCAMLFLAGS) -I $(MLGMPIDL_INSTALL)/lib -I $(APRON_PREFIX)/lib -o $@ bigarray.cma gmp.cma apron.cma polka%.cma ppl.cma polkaGrid.cma $<

mlexample%.opt: mlexample.ml $(APRON_PREFIX)/lib/polka%.cmxa $(APRON_PREFIX)/lib/ppl.cmxa polkaGrid.cmxa
	$(OCAMLOPT) $(OCAMLOPTFLAGS) -I $(MLGMPIDL_INSTALL)/lib -I $(APRON_PREFIX)/lib -o $@ bigarray.cmxa gmp.cmxa apron.cmxa polka%.cmxa ppl.cmxa polkaGrid.cmxa $<

clean:
	/bin/rm -f *.[ao] *.so *.cm[xiao] *.cmxa
	/bin/rm -f *.?.tex *.log *.aux *.bbl *.blg *.toc *.dvi *.ps *.pstex*

mostlyclean: clean
	/bin/rm -fr scalar.idl interval.idl coeff.idl dim.idl linexpr0.idl lincons0.idl generator0.idl texpr0.idl tcons0.idl manager.idl abstract0.idl macros.m4 sedscript_c
	/bin/rm -f polkaGrid.ml polkaGrid.mli polkaGrid_caml.c

install:
	$(INSTALLd) $(PREFIX)/include $(PREFIX)/lib
	$(INSTALL) $(CCINC_TO_INSTALL) $(PREFIX)/include
	for i in $(CCLIB_TO_INSTALL); do \
		if test -f $$i; then $(INSTALL) $$i $(PREFIX)/lib; fi; \
	done
	for i in dllpolkaGrid_caml*.so; do \
		if test -f $$i; then cp -f -d $$i $(PREFIX)/lib; fi; \
	done

uninstall:
	for i in $(CCINC_TO_INSTALL); do /bin/rm -f $(PREFIX)/include/$$i; done
	for i in $(CCLIB_TO_INSTALL); do /bin/rm -f $(PREFIX)/lib/$$i; done
	for i in dllpolkaGrid_caml*.so; do /bin/rm -f $(PREFIX)/lib/$$i; done
	for i in $(CCBIN_TO_INSTALL); do /bin/rm -f $(PREFIX)/bin/$$i; done

distclean: uninstall
	/bin/rm -f Makefile.depend

dist: $(CCSRC) ap_pkgrid.texi sedscript_caml polkaGrid.idl polkaGrid.ml polkaGrid.mli polkaGrid_caml.c Makefile COPYING README
	(cd ..; tar zcvf products.tgz $(^:%=products/%))

#---------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#---------------------------------------

.SUFFIXES: .tex .c .h .a .o

#-----------------------------------
# C part
#-----------------------------------

libap_pkgrid.a: ap_pkgrid.o
	$(AR) rcs $@ $^
	$(RANLIB) $@
libap_pkgrid_debug.a: ap_pkgrid_debug.o
	$(AR) rcs $@ $^
	$(RANLIB) $@
libap_pkgrid.so: ap_pkgrid.o
	$(CXX) $(CXXFLAGS) -shared -o $@ $^
libap_pkgrid_debug.so: ap_pkgrid_debug.o
	$(CXX) $(CXXFLAGS_DEBUG) -shared -o $@ $^

#---------------------------------------
# C rules
#---------------------------------------

libpolkaGrid_caml.a: polkaGrid_caml.o
	$(AR) rcs $@ $^
	$(RANLIB) $@
libpolkaGrid_caml_debug.a: polkaGrid_caml_debug.o
	$(AR) rcs $@ $^
	$(RANLIB) $@
libpolkaGrid_caml.so: polkaGrid_caml.o libap_pkgrid.so
	$(CXX) $(CXXFLAGS) -shared -o $@ $< -lap_pkgrid -L. -Wl-rpath,$(APRON_PREFIX)/lib
libpolkaGrid_caml_debug.so: polkaGrid_caml_debug.o libap_pkgrid_debug.so
	$(CXX) $(CXXFLAGS) -shared -o $@ $< -lap_pkgrid_debug -L. -Wl-rpath,$(APRON_PREFIX)/lib
dllpolkaGrid_caml.so: libpolkaGrid_caml.so
	ln -s -f $^ $@
dllpolkaGrid_caml_debug.so: libpolkaGrid_caml_debug.so
	ln -s -f $^ $@

#---------------------------------------
# ML rules
#---------------------------------------

polkaGrid.cma: polkaGrid.cmo libpolkaGrid_caml.a libap_pkgrid.a
	$(OCAMLMKLIB) -ocamlc "$(OCAMLC) -cc $(CXX)" -verbose -o polkaGrid -oc ap_polkaGrid_caml polkaGrid.cmo -lap_polkaGrid -L$(APRON_PREFIX)/lib -Wl,-rpath,$(APRON_PREFIX)/lib

polkaGrid.cmxa: polkaGrid.cmx libpolkaGrid_caml.a libap_pkgrid.a
	$(OCAMLMKLIB) -ocamlopt "$(OCAMLOPT) -cc $(CXX)" -verbose -o polkaGrid -oc ap_polkaGrid_caml polkaGrid.cmx -lap_polkaGrid -L$(APRON_PREFIX)/lib -Wl,-rpath,$(APRON_PREFIX)/lib

#---------------------------------------
# IDL rules
#---------------------------------------

scalar.idl: ../mlapronidl/scalar.idl
	ln -s $< $@
interval.idl: ../mlapronidl/interval.idl
	ln -s $< $@
coeff.idl: ../mlapronidl/coeff.idl
	ln -s $< $@
dim.idl: ../mlapronidl/dim.idl
	ln -s $< $@
linexpr0.idl: ../mlapronidl/linexpr0.idl
	ln -s $< $@
lincons0.idl: ../mlapronidl/lincons0.idl
	ln -s $< $@
generator0.idl: ../mlapronidl/generator0.idl
	ln -s $< $@
texpr0.idl: ../mlapronidl/texpr0.idl
	ln -s $< $@
tcons0.idl: ../mlapronidl/tcons0.idl
	ln -s $< $@
manager.idl: ../mlapronidl/manager.idl
	ln -s $< $@
abstract0.idl: ../mlapronidl/abstract0.idl
	ln -s $< $@
macros.m4: ../mlapronidl/macros.m4
	ln -s $< $@
sedscript_c: ../mlapronidl/sedscript_c
	ln -s $< $@

# generates polkaGrid.ml, polkaGrid.mli, polkaGrid_caml.c from polkaGrid.idl
rebuild: polkaGrid.idl macros.m4 sedscript_c scalar.idl interval.idl coeff.idl dim.idl linexpr0.idl lincons0.idl generator0.idl texpr0.idl tcons0.idl manager.idl abstract0.idl
	mkdir -p tmp
	cp polkaGrid.idl tmp/polkaGrid.idl
	$(CAMLIDL) -no-include -prepro "$(M4) --prefix-builtins macros.m4" tmp/polkaGrid.idl
	$(SED) -f sedscript_c tmp/polkaGrid_stubs.c >polkaGrid_caml.c
	$(SED) -f sedscript_caml tmp/polkaGrid.ml >polkaGrid.ml
	$(SED) -f sedscript_caml tmp/polkaGrid.mli >polkaGrid.mli

#---------------------------------------
# ML generic rules
#---------------------------------------

%.cmi: %.mli
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmo: %.ml %.cmi
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmx: %.ml %.cmi
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -c $<

#---------------------------------------
# C generic rules
#---------------------------------------

%.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_MPQ -c -o $@ $<
%_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_MPQ -c -o $@ $<
